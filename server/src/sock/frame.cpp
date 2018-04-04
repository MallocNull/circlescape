#include "frame.hpp"

static int calc_header_length(uint64_t body_length) {
    return 
        (body_length < 0x7E
            ? 2
            : (body_length <= 0xFFFF)
                ? 4
                : 10
        );
}

sosc::ws::Frame::Frame
    (const std::string& body, kOpCode opcode, bool final) 
{
    this->body = body;
    this->opcode = opcode;
    this->is_final = final;
}

int sosc::ws::Frame::Parse(const std::string& data, std::string* extra) {
    if(data.length() < 2)
        return FRAME_MORE;
    
    int header_length = Frame::GetHeaderLength(data);
    if(data.length() < header_length)
        return FRAME_MORE;
    
    uint64_t body_length = Frame::GetBodyLength(data);
    if(data.length() < header_length + body_length)
        return FRAME_MORE;
    
    this->is_final = (data[0] & 0x80) != 0;
    if((data[0] & 0x70) != 0)
        return FRAME_ERR;
    if(!IsValidOpCode(data[0] & 0x0F))
        return FRAME_ERR;
    
    this->opcode = static_cast<kOpCode>(data[0] & 0x0F);
    this->body = data.substr(header_length);
    
    if((data[1] & 0x80) != 0) {
        int offset = calc_header_length(body_length);
            
        std::string mask = data.substr(offset, 4);
        for(uint64_t i = 0; i < body_length; ++i)
            this->body[i] ^= mask[i % 4];
    }
    
    uint64_t expected_length = header_length + body_length;
    if(expected_length < data.length() && extra != nullptr)
        *extra = data.substr(expected_length);
    else if(extra != nullptr)
        *extra = "";
    
    return FRAME_OK;
}

int sosc::ws::Frame::GetHeaderLength(const std::string& data) {
    if(data.length() < 2)
        return -1;
    
    uint8_t length_byte = data[1] & 0x7F;
    return 2 +
        ((data[1] & 0x80) != 0 ? 4 : 0) +
        ((length_byte == 0x7E) ? 2 : 0) +
        ((length_byte == 0x7F) ? 8 : 0);
}

uint64_t sosc::ws::Frame::GetBodyLength(const std::string& data) {
    if(data.length() < 2)
        return -1;
    if(data.length() < Frame::GetHeaderLength(data))
        return -1;
    
    uint8_t length_byte = data[1] & 0x7F;
    return 
        (length_byte < 0x7E 
            ? length_byte
            : (length_byte == 0x7E
                ? net::ntohv<uint16_t>(data, 2)
                : net::ntohv<uint64_t>(data, 2)
            )
        );
}

bool sosc::ws::Frame::IsTooLarge(const std::string& data) {
    uint64_t header_length = Frame::GetHeaderLength(data),
             body_length = Frame::GetBodyLength(data);
             
    return (header_length + body_length) <= body_length;
}

void sosc::ws::Frame::PingFrame(Frame* frame) {
    frame->opcode = Ping;
    frame->is_final = true;
    frame->body = net::htonv<uint32_t>(0x600DB00B);
}

void sosc::ws::Frame::PongFrame(Frame* pingFrame) {
    pingFrame->opcode = Pong;
}

void sosc::ws::Frame::ClosingFrame
    (Frame* frame, kClosingReason status, const std::string& reason) 
{
    frame->opcode = Close;
    frame->is_final = true;
    frame->body = net::htonv<uint16_t>(status);
}

std::string* sosc::ws::Frame::ToString(std::string* frame) const {
    uint64_t body_length = this->body.length();
    *frame = std::string(2, 0);
    
    (*frame)[0] = (this->is_final ? 0x80 : 0x00) | this->opcode;
    if(body_length < 0x7E) 
        (*frame)[1] = body_length;
    else if(body_length < 0xFFFF) {
        (*frame)[1] = 0x7E;
        *frame += net::htonv<uint16_t>(body_length);
    } else {
        (*frame)[1] = 0x7F;
        *frame += net::htonv<uint64_t>(body_length);
    }
    
    *frame += this->body;
    return frame;
}

std::string sosc::ws::Frame::ToString() const {
    std::string frame;
    this->ToString(&frame);
    return frame;
}
