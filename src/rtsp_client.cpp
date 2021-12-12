#include "rtsp_client.hpp"
#include <iostream>
#include <chrono>
#include <boost/algorithm/string.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
namespace rtsp
{
    namespace client
    {
        using boost::asio::ip::tcp;

        client::client(const std::string& address, uint16_t port)
            : address_(address), port_(port)
        {
            // Get a list of endpoints corresponding to the server name.
            tcp::resolver resolver(io_service_);
            tcp::resolver::query query(address_, std::to_string(port_));
            tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
            tcp::resolver::iterator end;

            // Try each endpoint until we successfully establish a connection.
            boost::system::error_code error;
            boost::asio::connect(socket_, endpoint_iterator, end, error);
            if (error)
                throw boost::system::system_error(error);
            std::cout << endpoint_iterator->endpoint() << "connected" << std::endl;
        }

        rtsp::client::response client::options(
            const std::string& url)
        {
            url_ = url;
            do_write("OPTIONS", url_);
            return do_read();
        }

        rtsp::client::response client::describe(const std::map<std::string, std::string>& headers)
        {
            do_write("DESCRIBE", url_, headers);
            return do_read();
        }

        rtsp::client::response client::setup(const std::string& url, const std::map<std::string, std::string>& headers)
        {
            url_ = url;
            do_write("SETUP", url_, headers);
            return do_read();
        }

        rtsp::client::response client::play(const std::string& url, const std::map<std::string, std::string>& headers)
        {
            url_ = url;
            do_write("PLAY", url_, headers);
            return do_read();
        }

        rtsp::client::response client::teardown()
        {
            do_write("TEARDOWN", url_);
            return do_read();
        }

        void client::do_write(const std::string& method, const std::string& url,
            std::map<std::string, std::string> headers)
        {
            boost::asio::streambuf request;
            std::ostream request_stream(&request);

            request_stream << method << " " << url << " RTSP/1.0\r\n";
            for (auto& h : headers)
            {
                request_stream << h.first << ": " << h.second << "\r\n";
            }
            if (false == session_.empty())
            {
                request_stream << "Session: " << session_ << "\r\n";
            }
            request_stream << "CSeq: " << ++cseq_ << "\r\n";
            request_stream << "\r\n";

            // Send the request.
            //std::cout << "sending request...\n"
            //          << &request << "\n";
            std::cout << method << " - sent size: " << boost::asio::write(socket_, request) << std::endl;
        }

        response client::do_read()
        {
            boost::asio::streambuf buf;
            boost::asio::read_until(socket_, buf, "\r\n");
            // Check that buf is OK.
            std::istream response_stream(&buf);
            std::string rtsp_version;
            response_stream >> rtsp_version;
            unsigned int status_code;
            response_stream >> status_code;
            std::string status_message;

            std::getline(response_stream, status_message);
            std::stringstream ss;
            ss << rtsp_version << " " << status_code << status_message;
            response res;
            res.status_line = ss.str();
            if (!response_stream)
            {
                std::cout << "Invalid buf\n";
                throw std::runtime_error("invalid buf");
            }

            if (rtsp_version.substr(0, 5) != "RTSP/")
            {
                std::cout << "response_stream: " << &response_stream << std::endl;
                throw std::runtime_error("invalid buf");
            }

            // Read the buf headers, which are terminated by a blank line.
            boost::asio::read_until(socket_, buf, "\r\n\r\n");

            // Process the buf headers.
            std::string header;
            int content_length = 0;
            while (std::getline(response_stream, header) && header != "\r")
            {
                std::string name = header.substr(0, header.find(":"));
                std::string value = header.substr(header.find(":") + 1);
                value = value.substr(0, value.find("\r"));
                boost::algorithm::trim(value);

                if (name == "Content-Length")
                {
                    content_length = std::stoi(value);
                }
                if (name == "Session")
                {
                    session_ = value;
                }
                res.headers[name] = value;
            }

            //Write whatever content we already have to output.
            std::stringstream ss_content;
            ss_content << &buf;
            if (buf.size() > 0) ss_content << &buf;
            content_length -= buf.size();
            if (content_length > 0)
            {
                boost::asio::read_until(socket_, buf, "\r\n");
                ss_content << &buf;
            }
            const std::string bufContent(ss_content.str());
            res.content = ss_content.str();
            return res;
        }

    } // namespace client
} // namespace rtsp

std::ostream& operator<<(std::ostream& os, const rtsp::client::response& res)
{
    os << res.status_line << "\n";
    for (auto& h : res.headers)
    {
        os << h.first << ": " << h.second << "\n";
    }
    os << res.content << "\n";
    return os;
}
