#include <CL/opencl.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <array>

// Função para ler o arquivo de código-fonte do kernel OpenCL
std::string ler_codigo_kernel(const std::string &nome_arquivo)
{
    std::ifstream arquivo(nome_arquivo);
    std::stringstream buffer;
    buffer << arquivo.rdbuf();
    return buffer.str();
}

int main() {
    // Configurar o contexto OpenCL, plataforma e dispositivo
    const int LARGURA = 800;
    const int ALTURA = 600;
    cl_int erro;
    
    std::vector<cl::Platform> plataformas;
    std::vector<cl::Device> dispositivos;
    
    // Obter plataformas disponíveis
    cl::Platform::get(&plataformas);
    plataformas.front().getDevices(CL_DEVICE_TYPE_GPU, &dispositivos);

    // Verificar se há plataformas
    if (plataformas.empty()) {
        std::wcerr << "Não foi possível encontrar plataformas" << std::endl;
        return EXIT_FAILURE;
    }

    // Verificar se há dispositivos GPU
    if (dispositivos.empty()) {
        std::wcerr << "Não foi possível encontrar dispositivos GPU" << std::endl;
        return EXIT_FAILURE;
    }

    // Exibir o nome de cada plataforma
    for (auto plataforma : plataformas) {
        std::cout << plataforma.getInfo<CL_PLATFORM_NAME>().data() << std::endl;
    }

    // Exibir o nome de cada dispositivo
    for (auto dispositivo : dispositivos) {
        std::cout << dispositivo.getInfo<CL_DEVICE_NAME>().data() << std::endl;
    }

    // Criar o contexto OpenCL usando o primeiro dispositivo
    cl::Context contexto(dispositivos.front());
    cl::Program programa(contexto, ler_codigo_kernel("hello_triangle.cl"), CL_TRUE, &erro);

    if (erro != 0) {
        std::wcout << "[OpenCL] Erro no programa número: " << erro << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Programa: " << erro << std::endl;

    // Criar o kernel a partir do programa
    cl::Kernel kernel(programa, "tracar_raio", &erro);
    if (erro != 0) {
        std::wcout << "[OpenCL] Erro no kernel número: " << erro << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Kernel: " << erro << std::endl;

    size_t tamanho_buff = sizeof(cl_uchar3) * LARGURA * ALTURA;
    cl::Buffer buffer_imagem(contexto, CL_MEM_WRITE_ONLY, tamanho_buff, nullptr, &erro);

    // Configurar os argumentos do kernel
    kernel.setArg(0, buffer_imagem);
    kernel.setArg(1, LARGURA);
    kernel.setArg(2, ALTURA);

    // Criar fila de comandos para execução do kernel
    cl::CommandQueue fila(contexto, dispositivos.front());

    // Definir o tamanho global do trabalho
    cl::NDRange tamanho_trabalho_global(LARGURA * ALTURA, 1);

    // Enfileirar a execução do kernel
    fila.enqueueNDRangeKernel(kernel, cl::NullRange, tamanho_trabalho_global, cl::NullRange);
    fila.finish();

    // Criar vetor para armazenar a saída da renderização
    std::vector<cl_uchar4> buffer_saida(LARGURA * ALTURA);
    
    // Ler os dados do buffer de imagem
    fila.enqueueReadBuffer(buffer_imagem, CL_TRUE, 0, tamanho_buff, buffer_saida.data());

    // Salvar a imagem no formato PPM
    std::ofstream ppm("saida.ppm");
    ppm << "P3\n" << LARGURA << " " << ALTURA << "\n255\n";
    
    for (auto &pixel : buffer_saida)
    {
        ppm << (int)pixel.s[0] << " " << (int)pixel.s[1] << " " << (int)pixel.s[2] << " ";
    }

    std::wcout << "Renderização concluída. Salvo como saida.ppm\n";
    return 0;
}
