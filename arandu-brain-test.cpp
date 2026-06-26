#include <iostream>
#include <string>
#include <curl/curl.h> // Inclui a biblioteca cURL para fazer requisições web

// Função obrigatória do cURL para salvar o texto que a API do Gemini envia de volta
size_t SalvarResposta(void* conteudo, size_t tamanho, size_t num_elementos, void* string_destino) {
    // Adiciona o texto recebido da internet diretamente na nossa string do C++
    ((std::string*)string_destino)->append((char*)conteudo, tamanho * num_elementos);
    return tamanho * num_elementos; // Retorna o total de bytes processados
}

int main() {
    // CONFIGURAÇÃO DOS DADOS DE ACESSO
    std::string apiKey = "SUA_API_KEY_AQUI"; // <-- COLE SUA CHAVE DA API AQUI
    std::string modelo = "gemini-1.5-flash"; // Nome do modelo que vai processar o pedido

    // CONFIGURAÇÃO DO PRE-PROMPT E PROMPT
    std::string pre_prompt = "Você é um programador sênior focado em C++. Responda de forma curta."; // <-- SEU PRE-PROMPT AQUI (REGRAS)
    std::string prompt = "Como fazer um loop for?"; // <-- SEU PROMPT AQUI (PERGUNTA DO USUÁRIO)

    // MONTAGEM DO CORPO DA REQUISIÇÃO (PAYLOAD) EM FORMATO JSON
    // O Gemini exige uma estrutura específica de "system_instruction" e "contents"
    std::string json_dados = "{"
        "\"system_instruction\": {\"parts\": [{\"text\": \"" + pre_prompt + "\"}]},"
        "\"contents\": [{\"parts\": [{\"text\": \"" + prompt + "\"}]}]"
    "}";

    // Inicializa as variáveis necessárias do cURL
    CURL* curl; // Ponteiro para o objeto cURL
    CURLcode resultado; // Variável para checar se houve erros na conexão
    std::string resposta_bruta; // String que vai guardar o texto JSON que o Gemini responder

    curl_global_init(CURL_GLOBAL_ALL); // Inicializa os recursos globais do cURL
    curl = curl_easy_init(); // Inicia uma sessão de envio/recebimento simples

    if(curl) { // Verifica se o cURL foi iniciado corretamente
        // Cria o endereço da API juntando o modelo escolhido e a sua chave secreta
        std::string url = "https://googleapis.com" + modelo + ":generateContent?key=" + apiKey;

        // Configura o cabeçalho HTTP avisando a API que estamos enviando um texto em formato JSON
        struct curl_slist* cabecalhos = NULL;
        cabecalhos = curl_slist_append(cabecalhos, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // Informa o endereço web de destino ao cURL
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, cabecalhos); // Adiciona o cabeçalho configurado acima
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_dados.c_str()); // Insere o JSON (Pre-prompt + Prompt) na requisição
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, SalvarResposta); // Diz ao cURL para usar nossa função de salvar texto
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resposta_bruta); // Define onde o texto final deve ser guardado

        resultado = curl_easy_perform(curl); // Executa a ação de enviar os dados e aguarda a resposta da API

        if(resultado != CURLE_OK) { // Se o resultado for diferente de "OK", algo falhou na conexão
            std::cerr << "Erro na conexão: " << curl_easy_strerror(resultado) << std::endl;
        } else { // Se der certo, exibe o JSON recebido completo na tela
            std::cout << "Resposta Bruta do Gemini:\n" << resposta_bruta << std::endl;
        }

        curl_easy_cleanup(curl); // Fecha a sessão atual do cURL para liberar memória
        curl_slist_free_all(cabecalhos); // Limpa a lista de cabeçalhos da memória
    }

    curl_global_cleanup(); // Finaliza os recursos globais do cURL antes de fechar o programa
    return 0; // Termina o programa com sucesso
}
