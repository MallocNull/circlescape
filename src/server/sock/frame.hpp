#ifndef SOSC_WS_FRAME_H
#define SOSC_WS_FRAME_H

#include <string>
#include <cstdint>
#include "utils/net.hpp"

#define FRAME_OK    0
#define FRAME_MORE  1
#define FRAME_ERR  -1

namespace sosc {
namespace ws {
class Frame {
public:
    enum kOpCode {
        Continuation  = 0x0,
        TextFrame     = 0x1,
        BinaryFrame   = 0x2,
        Close         = 0x8,
        Ping          = 0x9,
        Pong          = 0xA
    };
    
    enum kClosingReason {
        Normal          = 1000,
        GoingAway       = 1001,
        ProtocolError   = 1002,
        FrameTypeError  = 1003,
        DataError       = 1007,
        PolicyError     = 1008,
        FrameTooBig     = 1009,
        RequestError    = 1011
    };
    
    Frame() {};
    Frame(const std::string& body, kOpCode opcode = BinaryFrame, 
          bool final = true);
    
    int Parse(const std::string& data, std::string* extra = nullptr);
    
    static int GetHeaderLength(const std::string& data);
    static uint64_t GetBodyLength(const std::string& data);
    static bool IsTooLarge(const std::string& data);
    
    static void PingFrame(Frame* frame);
    static void PongFrame(Frame* pingFrame);
    static void ClosingFrame
        (Frame* frame, kClosingReason status = Normal, 
         const std::string& reason = "");
        
    inline bool IsFinal() const {
        return this->is_final;
    }
    
    inline kOpCode GetOpCode() const {
        return this->opcode;
    }
    
    inline const std::string& GetBody() const {
        return this->body;
    }
    
    std::string* ToString(std::string* frame) const;
    std::string ToString() const;
    inline operator std::string () const {
        return this->ToString();
    }
private:
    kOpCode opcode;
    bool is_final;
    std::string body;
    
    inline static bool IsValidOpCode(int op) {
        return  (op == 0x0 || op == 0x1 || op == 0x2
              || op == 0x8 || op == 0x9 || op == 0xA);
    }
};
}}

#endif
