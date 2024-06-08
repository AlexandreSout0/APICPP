#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <iostream>

namespace beast = boost::beast;      // from <boost/beast.hpp>
namespace http = beast::http;        // from <boost/beast/http.hpp>
namespace net = boost::asio;         // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;    // from <boost/asio/ip/tcp.hpp>

void do_session(tcp::socket& socket) {
    try {
        for (;;) {
            // This buffer is required to persist across reads
            beast::flat_buffer buffer;

            // Read a request
            http::request<http::dynamic_body> request;
            http::read(socket, buffer, request);

            // Prepare and send the response
            http::response<http::dynamic_body> response{http::status::ok, request.version()};
            response.set(http::field::server, "Boost Beast");

            // Set the body of the response
            response.body() = "\r\n";

            // Prepare the response and write it to the socket
            response.prepare_payload();
            http::write(socket, response);
        }
    } catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    try {
        net::io_context ioc{1};

        tcp::acceptor acceptor{ioc, {tcp::v4(), 8080}};
        for (;;) {
            tcp::socket socket{ioc};
            acceptor.accept(socket);
            std::thread{[&socket] { do_session(socket); }}.detach();
        }
    } catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
