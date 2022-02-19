// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file HelloWorldPublisher.cpp
 *
 */

#include "HelloWorldPublisher.h"
#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/PublisherAttributes.h>
#include <fastrtps/publisher/Publisher.h>
#include <fastrtps/transport/TCPv4TransportDescriptor.h>
#include <fastrtps/Domain.h>
#include <fastrtps/utils/IPLocator.h>
#include"base64.h"
#include <thread>
#include <sstream>
#include <fstream>
#include <string>
 #include <chrono>
#include <thread>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;

HelloWorldPublisher::HelloWorldPublisher()
    : participant_(nullptr)
    , publisher_(nullptr)
{
}

bool HelloWorldPublisher::init(
        const std::string& wan_ip,
        unsigned short port,
        bool use_tls,
        const std::vector<std::string>& whitelist)
{
    stop_ = false;
    hello_.index(0);
    hello_.message("RZF");
    ParticipantAttributes pparam;

    pparam.rtps.builtin.discovery_config.leaseDuration = c_TimeInfinite;
    pparam.rtps.builtin.discovery_config.leaseDuration_announcementperiod = Duration_t(5, 0);
    pparam.rtps.setName("Participant_pub");

    pparam.rtps.useBuiltinTransports = false;

    std::shared_ptr<TCPv4TransportDescriptor> descriptor = std::make_shared<TCPv4TransportDescriptor>();

    for (std::string ip : whitelist)
    {
        descriptor->interfaceWhiteList.push_back(ip);
        std::cout << "Whitelisted " << ip << std::endl;
    }

    if (use_tls)
    {
        using TLSOptions = TCPTransportDescriptor::TLSConfig::TLSOptions;
        descriptor->apply_security = true;
        descriptor->tls_config.password = "test";
        descriptor->tls_config.cert_chain_file = "server.pem";
        descriptor->tls_config.private_key_file = "server.pem";
        descriptor->tls_config.tmp_dh_file = "dh2048.pem";
        descriptor->tls_config.add_option(TLSOptions::DEFAULT_WORKAROUNDS);
        descriptor->tls_config.add_option(TLSOptions::SINGLE_DH_USE);
        descriptor->tls_config.add_option(TLSOptions::NO_SSLV2);
    }

    descriptor->sendBufferSize = 0;
    descriptor->receiveBufferSize = 0;

    if (!wan_ip.empty())
    {
        descriptor->set_WAN_address(wan_ip);
        std::cout << wan_ip << ":" << port << std::endl;
    }
    descriptor->add_listener_port(port);
    pparam.rtps.userTransports.push_back(descriptor);

    participant_ = Domain::createParticipant(pparam);

    if (participant_ == nullptr)
    {
        return false;
    }
    //REGISTER THE TYPE

    Domain::registerType(participant_, &type_);

    //CREATE THE PUBLISHER
    PublisherAttributes wparam;
    wparam.topic.topicKind = NO_KEY;
    wparam.topic.topicDataType = "HelloWorld";
    wparam.topic.topicName = "HelloWorldTopicTCP";
    wparam.topic.historyQos.kind = KEEP_LAST_HISTORY_QOS;
    wparam.topic.historyQos.depth = 30;
    wparam.topic.resourceLimitsQos.max_samples = 50;
    wparam.topic.resourceLimitsQos.allocated_samples = 20;
    wparam.times.heartbeatPeriod.seconds = 2;
    wparam.times.heartbeatPeriod.nanosec = 200 * 1000 * 1000;
    wparam.qos.m_reliability.kind = RELIABLE_RELIABILITY_QOS;
    publisher_ = Domain::createPublisher(participant_, wparam, (PublisherListener*)&listener_);
    if (publisher_ == nullptr)
    {
        return false;
    }

    return true;
}

HelloWorldPublisher::~HelloWorldPublisher()
{
    Domain::removeParticipant(participant_);
}

void HelloWorldPublisher::PubListener::onPublicationMatched(
        Publisher*,
        MatchingInfo& info)
{
    if (info.status == MATCHED_MATCHING)
    {
        n_matched++;
        firstConnected = true;
        //logError(HW, "Matched");
        std::cout << "[RTCP] Publisher matched" << std::endl;
    }
    else
    {
        n_matched--;
        std::cout << "[RTCP] Publisher unmatched" << std::endl;
    }
}
int count = 0;

void HelloWorldPublisher::runThread(
        uint32_t samples,
        long sleep_ms)
{
    if (samples == 0)
    {
        while (!stop_)
        {
             HelloWorld hello_3;
            std::string mp3_contents;
            std::stringstream ss1;
            // ss1<<std::setw(4)<<std::setfill('0')<<i+1;
             std::string name_mp3= "1.mp3";
            std::cout<<name_mp3<<std::endl;
            std::ifstream t(name_mp3);
            std::stringstream buffer;
            buffer << t.rdbuf();
            std::string contents(buffer.str());
            std::cout<<contents.size()<<std::endl;
            mp3_contents = contents;
            if(mp3_contents.empty())
            {
                    std::cout<<"name_mp3 empty"<<name_mp3<<std::endl;
                    break;
            }
            hello_3.index(count);
            count++;
            std::string send_data;
            std::cout<<mp3_contents[10000]<<std::endl;
             mp3_contents.insert(mp3_contents.size(),"mp3");

             
       
            std::vector<u_char> v(mp3_contents.begin(),mp3_contents.end());
           
                //  std::cout<<"hello_3.message() "<<send_data<<std::endl;
            std::cout<<"hello_3.message()  size "<<send_data.size()<<std::endl;

             
            cv::Mat img = cv::imread("4.jpg",1); //CV_LOAD_IMAGE_GRAYSCALE
            if(img.empty())
            {
                std::cout<<"name"<<std::endl;
                    std::cout<<"img empty"<<std::endl;
                     break;
            }
             
            cv::imshow("Publisher",img);
            cv::waitKey(40);
            std::string encoding;
            std::string send_string;
             std::vector<uchar>   v2;
             std::vector<uchar> m_data;
              cv::imencode(".jpg"+encoding, img, m_data);
            std::string send_string1;
            std::stringstream out;
            std::vector<uchar> m_data1;
            // for (int i = 0; i < m_data.size(); i++)
            // {
            //     m_data1.push_back(m_data[i]);
            //     // out << (uchar)m_data[i];
            //     // out << (uchar)i %255;
                
            // }
           
                for (int i = 0; i < m_data.size(); i++)
                {
                   
                    out << (uchar)m_data[i];
                    // out << (uchar)i %255;
                    
                }
                
              std::cout<<mp3_contents.size()<<std::endl;
              std::string  base64_str = base64_encode(mp3_contents);
              std::cout<<base64_str.size()<<std::endl;

              std::string  base64_str1 = base64_decode(base64_str);
              std::cout<<base64_str1.size()<<std::endl;
                 for (int i = 0; i < 50; i++)
            {
                /* code */
                std::cout<<mp3_contents[i];
                // send_data.insert(0,std::to_string(i));
            }
             std::cout  << std::endl;
                 for (int i = 0; i < 50; i++)
            {
                /* code */
                std::cout<<base64_str[i];
                // send_data.insert(0,std::to_string(i));
            }
                 std::cout  << std::endl;
             for (int i = 0; i < 50; i++)
            {
                /* code */
                std::cout<<base64_str1[i];
                // send_data.insert(0,std::to_string(i));
            }
            std::cout  << std::endl;
               send_string1 +=out.str();
               
               std::string jpg_base64 = base64_encode(send_string1);

            //    for (size_t i = 0; i < jpg_base64.size(); i++)
            //  {
            //      m_data1.push_back((uchar)jpg_base64[i]);
            //  }

                // std::string s(m_data.begin(), m_data.end()- 4);
                std::string jpg_base641 = base64_decode(jpg_base64);
                std::vector<uchar> v11(jpg_base641.begin(),jpg_base641.end());
              cv::Mat  imgRes = cv::imdecode(v11, 1);    
            //  if(!imgRes.empty())      
            //  {
            //            cv::imshow("Subscriber", imgRes);
            //             cv::waitKey(40);
                
            //  }
             
             std::cout<<"Subscriber"<<std::endl;
             std::cout<<m_data.size()<<std::endl;
             std::cout<<jpg_base641.size()<<std::endl;
            hello_3.message(jpg_base64);
             if (publish1(hello_3))
            {
                //  std::cout<<"hello_3.message() "<<hello_3.message()<<std::endl;
                //logError(HW, "SENT " <<  hello_.index());
                std::cout << "[RTCP] Message: "  <<hello_3.message().size()<< " with index: "
                << hello_3.index() << " SENT" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
        }
    }
    else
    {
        for (uint32_t i = 0; i < samples; ++i)
        {
            hello_.message(std::to_string(i)+"raozhufa");
            HelloWorld hello_2;
            hello_2.index(0);
            hello_2.message("tttt");
            if (!publish1(hello_2))
            {
                --i;
            }
            else
            {
                std::cout << "[RTCP] Message: " << hello_2.message() << " with index: "
                          << hello_2.index() << " SENT" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
        }
    }
}

void HelloWorldPublisher::run(
        uint32_t samples,
        long sleep_ms)
{
    std::thread thread(&HelloWorldPublisher::runThread, this, samples, sleep_ms);
    if (samples == 0)
    {
        std::cout << "Publisher running. Please press enter to stop_ the Publisher at any time." << std::endl;
        std::cin.ignore();
        stop_ = true;
    }
    else
    {
        std::cout << "Publisher running " << samples << " samples." << std::endl;
    }
    thread.join();
}

bool HelloWorldPublisher::publish(
        bool waitForListener)
{
    if (listener_.firstConnected || !waitForListener || listener_.n_matched > 0)
    {
        hello_.index(hello_.index() + 1);
        publisher_->write((void*)&hello_);
        return true;
    }
    return false;
}

bool HelloWorldPublisher::publish1(HelloWorld data)
{
    if (listener_.firstConnected ||  listener_.n_matched > 0)
    {
        data.index(data.index() + 1);
        publisher_->write((void*)&data);
        return true;
    }
    return false;
}
