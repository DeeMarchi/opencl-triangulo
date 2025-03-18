__kernel void tracar_raio(__global uchar4* imagem, const int largura, const int altura)
{
    // Posição da câmera (olhando ao longo do eixo Z negativo)
    float3 origem = { 0.0f, 0.0f, -1.0f };  // Câmera na origem, olhando ao longo do eixo -Z

    // Variáveis de direção do raio
    float3 direcao;
    
    int id = get_global_id(0);
    int x = id % largura;
    int y = id / largura;

    // Definir direção do raio com base no pixel (isto é simplificado)
    direcao.x = (2.0f * ((float)x / largura) - 1.0f);       // Mapeamento do FOV horizontal
    direcao.y = 1.0f - (2.0f * ((float)y / altura));       // Mapeamento do FOV vertical
    direcao.z = 1.0f; // Olhando ao longo do eixo Z

    // Normalizar a direção do raio
    float comprimento = sqrt(direcao.x * direcao.x + direcao.y * direcao.y + direcao.z * direcao.z);
    direcao.x /= comprimento;
    direcao.y /= comprimento;
    direcao.z /= comprimento;

    // Definir os vértices do triângulo (colocados à frente da câmera no espaço da visão)
    float3 v0 = { -1.0f, -1.0f, 2.0f };  // Vértice inferior esquerdo
    float3 v1 = {  1.0f, -1.0f, 2.0f };  // Vértice inferior direito
    float3 v2 = {  1.0f,  1.0f, 2.0f };  // Vértice superior

    // Calcular as arestas do triângulo
    float3 aresta1 = v1 - v0;
    float3 aresta2 = v2 - v0;

    // Teste de interseção do raio com o triângulo usando o algoritmo Möller–Trumbore
    float3 h = cross(direcao, aresta2);
    float a = dot(aresta1, h);

    // Se o raio for paralelo ao triângulo, não há interseção
    if (a > -1e-5f && a < 1e-5f)
    {
        imagem[id] = (uchar4)(0, 0, 0, 255);  // Preto (sem interseção)
        return;
    }

    float f = 1.0f / a;
    float3 s = origem - v0;
    float u = f * dot(s, h);

    if (u < 0.0f || u > 1.0f)
    {
        imagem[id] = (uchar4)(0, 0, 0, 255);  // Preto (sem interseção)
        return;
    }

    float3 q = cross(s, aresta1);
    float v = f * dot(direcao, q);

    if (v < 0.0f || u + v > 1.0f)
    {
        imagem[id] = (uchar4)(0, 0, 0, 255);  // Preto (sem interseção)
        return;
    }

    // Calcular o ponto de interseção (t é a distância da origem do raio)
    float t = f * dot(aresta2, q);
    if (t > 1e-5f)  // Se o raio intersecta o triângulo
    {
        imagem[id] = (uchar4)(255, 0, 0, 255);  // Colorir o pixel de vermelho
    }
    else
    {
        imagem[id] = (uchar4)(0, 0, 0, 255);  // Preto (sem colisão)
    }
}
