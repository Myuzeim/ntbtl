#include "move_machine.h"
#include <cstring>

namespace MoveMachine {
    Machine::Machine() {
        _movingBack = false;
        _initStateMap();
        _changeState(this,IDLE);
    };

    const Out& Machine::_idle(StateMachine<In,Out>* sm, const In& in) {
        Machine* m = static_cast<Machine*>(sm);
        m->_ret.clear();
        if(in.keys.Back() && m->_canMoveBack(in.px)) {
            m->_movingBack = true;
            if(in.keys.keys.Move1 && m->_canMoveNear(in.py)) {
                m->_moveFrames = DIAG_SPEED;
                m->_srcX = in.px;
                m->_dstX = in.px - TILE_WIDTH;
                m->_srcY = in.py;
                m->_dstY = in.py - TILE_HEIGHT;
                m->_changeState(m,  MOVING);
                
                m->_ret.px = m->_srcX;
                m->_ret.py = m->_srcY;
                m->_ret.inMove = true;
                m->_ret.movingBack = m->_movingBack;
                return m->_ret;
            } else if(in.keys.keys.Move7 && m->_canMoveFar(in.py)) {
                m->_moveFrames = DIAG_SPEED;
                m->_srcX = in.px;
                m->_dstX = in.px - TILE_WIDTH;
                m->_srcY = in.py;
                m->_dstY = in.py + TILE_HEIGHT;
                m->_changeState(m,  MOVING);
                
                m->_ret.px = m->_srcX;
                m->_ret.py = m->_srcY;
                m->_ret.inMove = true;
                m->_ret.movingBack = m->_movingBack;
                return m->_ret;
            } else { //Move4
                m->_moveFrames = HORI_SPEED;
                m->_srcX = in.px;
                m->_dstX = in.px - TILE_WIDTH;
                m->_srcY = in.py;
                m->_dstY = in.py;
                m->_changeState(m,  MOVING);
                
                m->_ret.px = m->_srcX;
                m->_ret.py = m->_srcY;
                m->_ret.inMove = true;
                m->_ret.movingBack = m->_movingBack;
                return m->_ret;
            };
        } else if(in.keys.Forw() && m->_canMoveForw(in.px)) {
            if(in.keys.keys.Move3 && m->_canMoveNear(in.py)) {
                m->_moveFrames = DIAG_SPEED;
                m->_srcX = in.px;
                m->_dstX = in.px + TILE_WIDTH;
                m->_srcY = in.py;
                m->_dstY = in.py - TILE_HEIGHT;
                m->_changeState(m,  MOVING);
                
                m->_ret.px = m->_srcX;
                m->_ret.py = m->_srcY;
                m->_ret.inMove = true;
                m->_ret.movingBack = m->_movingBack;
                return m->_ret;
            } else if(in.keys.keys.Move9 && m->_canMoveFar(in.py)) {
                m->_moveFrames = DIAG_SPEED;
                m->_srcX = in.px;
                m->_dstX = in.px + TILE_WIDTH;
                m->_srcY = in.py;
                m->_dstY = in.py + TILE_HEIGHT;
                m->_changeState(m,  MOVING);
                
                m->_ret.px = m->_srcX;
                m->_ret.py = m->_srcY;
                m->_ret.inMove = true;
                m->_ret.movingBack = m->_movingBack;
                return m->_ret;
            } else { //Move6
                m->_moveFrames = HORI_SPEED;
                m->_srcX = in.px;
                m->_dstX = in.px + TILE_WIDTH;
                m->_srcY = in.py;
                m->_dstY = in.py;
                m->_changeState(m, MOVING);
                
                m->_ret.px = m->_srcX;
                m->_ret.py = m->_srcY;
                m->_ret.inMove = true;
                m->_ret.movingBack = m->_movingBack;
                return m->_ret;
            };
        } else {
            if(in.keys.Near() && m->_canMoveNear(in.py)) {
                m->_moveFrames = VERT_SPEED;
                m->_srcX = in.px;
                m->_dstX = in.px;
                m->_srcY = in.py;
                m->_dstY = in.py - TILE_HEIGHT;
                m->_changeState(m,  MOVING);
                
                m->_ret.px = m->_srcX;
                m->_ret.py = m->_srcY;
                m->_ret.inMove = true;
                m->_ret.movingBack = m->_movingBack;
                return m->_ret;
            } else if(in.keys.Far() && m->_canMoveFar(in.py)) {
                m->_moveFrames = VERT_SPEED;
                m->_srcX = in.px;
                m->_dstX = in.px;
                m->_srcY = in.py;
                m->_dstY = in.py + TILE_HEIGHT;
                m->_changeState(m,  MOVING);
                
                m->_ret.px = m->_srcX;
                m->_ret.py = m->_srcY;
                m->_ret.inMove = true;
                m->_ret.movingBack = m->_movingBack;
                return m->_ret;
            }
        }
        //no move
        m->_srcX = in.px;
        m->_dstX = in.px;
        m->_srcY = in.py;
        m->_dstY = in.py; 

        m->_ret.px = m->_srcX;
        m->_ret.py = m->_srcY;
        m->_ret.inMove = false;
        m->_ret.movingBack = m->_movingBack;
        return m->_ret;
    };
    const Out& Machine::_moving(StateMachine<In,Out>* sm, const In& in) {
        Machine* m = static_cast<Machine*>(sm);
        m->_ret.clear();

        uint8_t curFrame = m->_currentFrame();
        if(curFrame >= m->_moveFrames) {
            m->_movingBack = false;
            m->_changeState(m, IDLE);
            m->_srcX = m->_dstX;
            m->_srcY = m->_dstY;
            m->_dstX = m->_dstX;
            m->_dstY = m->_dstY;

            m->_ret.px = m->_dstX;
            m->_ret.py = m->_dstY;
            m->_ret.inMove = false;
            m->_ret.movingBack = m->_movingBack;
            return m->_ret;
        };

        
        uint8_t curX, curY;
        if(m->_dstX < m->_srcX) {
            //Moving back
            curX = m->_srcX - (m->_srcX - m->_dstX) * (curFrame*1.0f/m->_moveFrames);
        } else {
            //Moving forw
            curX = m->_srcX + (m->_dstX - m->_srcX) * (curFrame*1.0f/m->_moveFrames);
        }
        if(m->_dstY < m->_srcY) {
            //Moving near
            curY = m->_srcY - (m->_srcY - m->_dstY) * (curFrame*1.0f/m->_moveFrames);
        } else {
            //Moving far
            curY = m->_srcY + (m->_dstY - m->_srcY) * (curFrame*1.0f/m->_moveFrames);
        }

        m->_ret.px = curX;
        m->_ret.py = curY;
        m->_ret.inMove = true;
        m->_ret.movingBack = m->_movingBack;
        return m->_ret;
    };
    size_t Machine::serialize(char* addr) {
        size_t offset = 0;
        State sEnum = (State) _currentEnum();
        offset += serializeBytes(addr+offset, sEnum);
        offset += serializeBytes(addr+offset,_currentFrame());
        offset += serializeBytes(addr+offset,_moveFrames);
        offset += serializeBytes(addr+offset,_srcX);
        offset += serializeBytes(addr+offset,_srcY);
        offset += serializeBytes(addr+offset,_dstX);
        offset += serializeBytes(addr+offset,_dstY);
        offset += serializeBytes(addr+offset,_movingBack);
        return offset;
    }

    Machine::Machine(char* addr, size_t& size) {
        _initStateMap();
        State sEnum = *reinterpret_cast<State*>(addr+size);
        size += sizeof(sEnum);
        switch(sEnum) {
            case IDLE:
                _changeState(this,IDLE);
                break;
            case MOVING:
                _changeState(this,MOVING);
                break;
            default:
                break; //error!
        }
        uint8_t frame = *reinterpret_cast<uint8_t*>(addr+size);
        size += sizeof(frame);
        _changeFrameOnly(frame);
        
        _moveFrames = *reinterpret_cast<uint8_t*>(addr+size);
        size += sizeof(_moveFrames);
        
        _srcX = *reinterpret_cast<uint8_t*>(addr+size);
        size += sizeof(_srcX);

        _srcY = *reinterpret_cast<uint8_t*>(addr+size);
        size += sizeof(_srcY);

        _dstX = *reinterpret_cast<uint8_t*>(addr+size);
        size += sizeof(_dstX);

        _dstY = *reinterpret_cast<uint8_t*>(addr+size);
        size += sizeof(_dstY);

        _movingBack = *reinterpret_cast<uint8_t*>(addr+size);
        size += sizeof(_movingBack);
    };

    void Machine::_initStateMap() {
        _enumToStateFunction[IDLE] = _idle;
        _enumToStateFunction[MOVING] = _moving;
    }
};