
#include "stdafx.h"

#include "media.h"
#include "source.h"
#include "rtsp_client.hpp"
#include <stdio.h>
#include <iostream>

uint32_t hexStringToInt(const std::string& hex)
{
    uint32_t dst;
    std::stringstream ss;
    ss << std::hex << hex;
    ss >> dst;
    return dst;
}


int main()
{
    using namespace boost::asio;

#if 0
    // For debugging RTCP receives
    media::source src1;
    int src1_rtp = 40000;
    int src1_rtcp = 40001;
    int src2_rtp = 40002;
    int src2_rtcp = 40003;
    src1.open_stream(ip::address_v4::any(), "source1", src1_rtp, src1_rtcp);
    src1.start();

    io_service io;
    ip::udp::socket remote(io, ip::udp::endpoint(ip::address_v4::any(), 40003));
    unsigned char data[] = {
        0x81, 0xc8, 0x00, 0x0c, 0x00, 0x00, 0x4a, 0x00, 
        0xd8, 0x90, 0xd3, 0xbe, 0x28, 0x72, 0xaf, 0xf2, 
        0x00, 0x00, 0x7f, 0x7e, 0x00, 0x00, 0x00, 0x5e, 
        0x00, 0x00, 0x0b, 0xea, 0x87, 0xd5, 0xe2, 0xd2,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa6, 
        0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00
    };
    remote.send_to(
        buffer(data),
        ip::udp::endpoint(ip::address::from_string("127.0.0.1"), 40001));
#endif

#if 0
    //For debugging RTCP sends
    media::rtp_service pair;
    media::rtcp tester(pair, 0xfefefefe, "my_cname");
    int port1 = 50000;
    int port2 = 50001;
    pair.open(ip::address_v4::any(), port1, port2);
    pair.set_rtcp_peer(ip::udp::endpoint(ip::address::from_string("192.168.1.2"), 60001));
    pair.start();

    printf("Press any key to stop media\n");
    getc(stdin);

    pair.stop();
    printf("Media stopped\n");
#endif

#if 0
    rtsp::client::client c("wowzaec2demo.streamlock.net", 554);
    std::string url("rtsp://wowzaec2demo.streamlock.net:554/vod/mp4:BigBuckBunny_115k.mov");
    auto res(c.options(url));
    std::cout << "options response: \n" << res << std::endl;
    res = c.describe(std::map<std::string, std::string>{ {"Accept", "application/sdp"}});
    std::cout << "describe response: \n" << res << std::endl;
    res = c.setup(url + "/trackID=1", std::map<std::string, std::string>{{"Transport", "RTP/AVP;unicast;client_port=62278-62279"}});
    std::cout << "setup audio response: \n" << res << std::endl;
    
    int localRtpAudioPort = 62278;
    int localRtcpAudioPort = 62279;

    std::string audioRemoteIp;
    std::cout << "remote ip:";
    std::cin >> audioRemoteIp;
    int audioRtpPort;
    std::cout << "audio rtp port: ";
    std::cin >> audioRtpPort;
    int audioRtcpPort;
    std::cout << "audio rtcp port: ";
    std::cin >> audioRtcpPort;
    const auto audioRemoteRtpEndpoint = ip::udp::endpoint(ip::address::from_string(audioRemoteIp), audioRtpPort);
    const auto audioRemoteRtcpEndpoint = ip::udp::endpoint(ip::address::from_string(audioRemoteIp), audioRtcpPort);

    media::source src;
    src.open_stream(ip::address_v4::any(), "audio", localRtpAudioPort, localRtcpAudioPort, audioRemoteRtpEndpoint, audioRemoteRtcpEndpoint);
    
    /*io_service io;
    media::rtp_service audioPair(io);
    std::string audioSsrc;
    std::cout << "audio ssrc: ";
    std::cin >> audioSsrc;
    media::rtcp audioTester(audioPair, hexStringToInt(audioSsrc), "audio");
    int port1 = 62278;
    int port2 = 62279;
    audioPair.open(ip::address_v4::any(), port1, port2);
    std::string audioRemoteIp;
    std::cout << "remote ip:";
    std::cin >> audioRemoteIp;
    int audioRtpPort;
    std::cout << "audio rtp port: ";
    std::cin >> audioRtpPort;
    audioPair.set_rtp_peer(ip::udp::endpoint(ip::address::from_string(audioRemoteIp), audioRtpPort));
    int audioRtcpPort;
    std::cout << "audio rtcp port: ";
    std::cin >> audioRtcpPort;
    audioPair.set_rtcp_peer(ip::udp::endpoint(ip::address::from_string(audioRemoteIp), audioRtcpPort));
    audioPair.start();*/
    
    res = c.setup(url + "/trackID=2", std::map<std::string, std::string>{ {"Transport", "RTP/AVP;unicast;client_port=62280-62281"}});
    std::cout << "setup video response: \n" << res << std::endl;
    
    int localRtpVideoPort = 62280;
    int localRtcpVideoPort = 62281;

    std::string videoRemoteIp;
    std::cout << "remote ip:";
    std::cin >> videoRemoteIp;
    int videoRtpPort;
    std::cout << "video rtp port: ";
    std::cin >> videoRtpPort;
    int videoRtcpPort;
    std::cout << "video rtcp port: ";
    std::cin >> videoRtcpPort;
    const auto videoRemoteRtpEndpoint(ip::udp::endpoint(ip::address::from_string(videoRemoteIp), videoRtpPort));
    const auto videoRemoteRtcpEndpoint(ip::udp::endpoint(ip::address::from_string(videoRemoteIp), videoRtcpPort));
    src.open_stream(ip::address_v4::any(), "video", localRtpVideoPort, localRtcpVideoPort, videoRemoteRtpEndpoint, videoRemoteRtcpEndpoint);

    src.start();

    /*media::rtp_service videoPair(io);
    std::string videoSsrc;
    std::cout << "video ssrc: ";
    std::cin >> videoSsrc;
    media::rtcp videoTester(videoPair, hexStringToInt(videoSsrc), "video");
    int port3 = 62280;
    int port4 = 62281;
    videoPair.open(ip::address_v4::any(), port3, port4);
    std::string videoRemoteIp;
    std::cout << "remote ip:";
    std::cin >> videoRemoteIp;
    int videoRtpPort;
    std::cout << "video rtp port: ";
    std::cin >> videoRtpPort;
    videoPair.set_rtp_peer(ip::udp::endpoint(ip::address::from_string(videoRemoteIp), videoRtpPort));
    int videoRtcpPort;
    std::cout << "video rtcp port: ";
    std::cin >> videoRtcpPort;
    videoPair.set_rtcp_peer(ip::udp::endpoint(ip::address::from_string(videoRemoteIp), videoRtcpPort));
    videoPair.start();*/
    
    res = c.play(url, std::map<std::string, std::string>{ {"Range", "npt=0.000-"}});
    std::cout << "play response: \n" << res << std::endl;

    

    //printf("Press any key to stop media\n");
    //getc(stdin);
    //std::cout << "press any key to exit"; getchar();
    Sleep(300000);

    res = c.teardown();
    std::cout << res;

    src.stop();
    //videoPair.stop();
    //audioPair.stop();
#endif

    rtsp::client::client c("ipcam.stream", 8554);
    std::string url("rtsp://ipcam.stream:8554/white");
    auto res(c.options(url));
    std::cout << "options response: \n" << res << std::endl;
    res = c.describe(std::map<std::string, std::string>{ {"Accept", "application/sdp"}});
    std::cout << "describe response: \n" << res << std::endl;
    res = c.setup(url + "/stream=0", std::map<std::string, std::string>{ {"Transport", "RTP/AVP;unicast;client_port=62278-62279"}});
    std::cout << "setup video response: \n" << res << std::endl;

    int localRtpAudioPort = 62278;
    int localRtcpAudioPort = 62279;

    std::string audioRemoteIp;
    std::cout << "remote ip:";
    std::cin >> audioRemoteIp;
    int audioRtpPort;
    std::cout << "audio rtp port: ";
    std::cin >> audioRtpPort;
    int audioRtcpPort;
    std::cout << "audio rtcp port: ";
    std::cin >> audioRtcpPort;
    const auto audioRemoteRtpEndpoint = ip::udp::endpoint(ip::address::from_string(audioRemoteIp), audioRtpPort);
    const auto audioRemoteRtcpEndpoint = ip::udp::endpoint(ip::address::from_string(audioRemoteIp), audioRtcpPort);

    media::source src;
    src.open_stream(ip::address_v4::any(), "audio", localRtpAudioPort, localRtcpAudioPort, audioRemoteRtpEndpoint, audioRemoteRtcpEndpoint);

    res = c.setup(url + "/stream=1", std::map<std::string, std::string>{ {"Transport", "RTP/AVP;unicast;client_port=62280-62281"}});
    std::cout << "setup audio response: \n" << res << std::endl;

    int localRtpVideoPort = 62280;
    int localRtcpVideoPort = 62281;

    std::string videoRemoteIp;
    std::cout << "remote ip:";
    std::cin >> videoRemoteIp;
    int videoRtpPort;
    std::cout << "video rtp port: ";
    std::cin >> videoRtpPort;
    int videoRtcpPort;
    std::cout << "video rtcp port: ";
    std::cin >> videoRtcpPort;
    const auto videoRemoteRtpEndpoint(ip::udp::endpoint(ip::address::from_string(videoRemoteIp), videoRtpPort));
    const auto videoRemoteRtcpEndpoint(ip::udp::endpoint(ip::address::from_string(videoRemoteIp), videoRtcpPort));
    src.open_stream(ip::address_v4::any(), "video", localRtpVideoPort, localRtcpVideoPort, videoRemoteRtpEndpoint, videoRemoteRtcpEndpoint);

    src.start();

    res = c.play(url, std::map<std::string, std::string>{ {"Range", "npt=0.000-"}});
    std::cout << "play response: \n" << res << std::endl;

    Sleep(300000);

    res = c.teardown();
    std::cout << res;

    src.stop();
}
