#include "Handler.h"
//#include "cpp-base64/base64.cpp"

using namespace std;

namespace videogame_store::handler {

    // Gestisce le richieste POST di registrazione al servizio
    void signup_handler(const std::shared_ptr<restbed::Session> session) {
        cout << "signup" << endl;
        const auto request = session->get_request();
        size_t content_length = request->get_header("Content-Length", 0);
        // Lettura dati e gestione richiesta HTTP
        session->fetch(content_length, [request](const std::shared_ptr< restbed::Session > session, const restbed::Bytes& body)
            {
                // Analizzo il payload associato alla richiesta POST e lo converto in un JSON
                nlohmann::json requestJson = nlohmann::json::parse(body.data());
                std::string username = requestJson["username"];
                std::string password = requestJson["password"];
                try {
                    // Registro l'utente sul database
                    MongoDB* mongoDb = MongoDB::getInstance();
                    mongoDb->signup(username, password);
                    // Restituisco all'utente un jwt token che verrà usato per autenticare l'utente per le varie richieste
                    jwt::jwt_object obj{ jwt::params::algorithm("HS256"), jwt::params::payload({{"username", username}}), jwt::params::secret("secret") };
                    // Aggiunta scadenza al token
                    obj.add_claim("exp", std::chrono::system_clock::now() + std::chrono::hours(2));
                    session->close(restbed::OK, obj.signature(), { { "Content-Length", to_string(obj.signature().length())}, {"Content-Type", "text/html"},{ "Connection", "close" } });
                }
                //TODO: SignupException
                //catch (SignupException& e) {
                catch (exception& e){
                    session->close(restbed::BAD_REQUEST, e.what(), { { "Content-Length", to_string(strlen(e.what()))}, {"Content-Type", "text/html"},{ "Connection", "close" } });
                }

            });
    }

    // Gestisce le richieste POST di login al servizio
    void login_handler(const std::shared_ptr<restbed::Session> session) {
        cout << "login" << endl;
        const auto request = session->get_request();
        size_t content_length = request->get_header("Content-Length", 0);
        // Lettura dati e gestione richiesta HTTP
        session->fetch(content_length, [request](const std::shared_ptr< restbed::Session > session, const restbed::Bytes& body)
            {
                // Analizzo il payload associato alla richiesta POST e lo converto in un JSON
                nlohmann::json requestJson = nlohmann::json::parse(body.data());
                std::string username = requestJson["username"];
                std::string password = requestJson["password"];
                try {
                    // Controllo se l'utente è presente nel database
                    MongoDB* mongoDb = MongoDB::getInstance();
                    mongoDb->login(username, password);
                    // Restituisco all'utente un jwt token che verrà usato per autenticare l'utente per le varie richieste di upload, get e like
                    jwt::jwt_object obj{ jwt::params::algorithm("HS256"), jwt::params::payload({{"username", username}}), jwt::params::secret("secret") };
                    obj.add_claim("exp", std::chrono::system_clock::now() + std::chrono::hours(2));
                    session->close(restbed::OK, obj.signature(), { { "Content-Length", to_string(obj.signature().length())}, {"Content-Type", "text/html"},{ "Connection", "close" } });
                }
                //TODO: SignupException
                //catch (LoginException& e) {
                catch (exception& e) {
                    session->close(restbed::BAD_REQUEST, "User non presente", { { "Content-Length", "17"}, {"Content-Type", "text/html"},{ "Connection", "close" } });
                }

            });
    }

    // Gestisce le richieste POST di upload
    void upload_handler(const std::shared_ptr< restbed::Session > session) {
        cout << "upload" << endl;
        std::string username;
        const auto request = session->get_request();
        size_t content_length = request->get_header("Content-Length", 0);
        auto token = request->get_header("Authorization", "");
        try {
            // Verifico che il token passato nel campo Authorization del header sia corretto; se così non fosse restituisco un errore al client
            auto dec_token = jwt::decode(token, jwt::params::algorithms({ "HS256" }), jwt::params::secret("secret"));
            username = dec_token.payload().get_claim_value<std::string>("username");
        }
        catch (const jwt::TokenExpiredError& e) {
            session->close(restbed::UNAUTHORIZED, e.what(), { { "Content-Length", to_string(strlen(e.what()))}, {"Content-Type", "text/html"},{ "Connection", "close" } });
            return;
        }
        //TODO: Implementare gestione richiesta
        /*session->fetch(content_length, [request, username](const std::shared_ptr< restbed::Session > session, const restbed::Bytes& body)
            {
                
            }*/
    }

    //TODO: Implementare altri metodi per gestione richieste

}