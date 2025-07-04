#include "move_machine.h"
#include <cstring>

namespace MoveMachine {
    Machine::Machine() {
        _movingBack = false;
        _changeState(this,_idle,IDLE);
    };

    const Out Machine::_idle(StateMachine<In,Out>* sm, const In& in) {
        Machine* m = static_cast<Machine*>(sm);
        if(in.keys.Back() && m->_canMoveBack(in.px)) {
            m->_movingBack = true;
            if(in.keys.keys.Move1 && m->_canMoveNear(in.py)) {
                m->_moveFrames = DIAG_SPEED;
                m->_srcX = in.px;
                m->_dstX = in.px - TILE_WIDTH;
                m->_srcY = in.py;
                m->_dstY = in.py - TILE_HEIGHT;
                m->_changeState(m, _moving, MOVING);
                return {m->_srcX,m->_srcY,true,m->_movingBack};
            } else if(in.keys.keys.Move7 && m->_canMoveFar(in.py)) {
                m->_moveFrames = DIAG_SPEED;
                m->_srcX = in.px;
                m->_dstX = in.px - TILE_WIDTH;
                m->_srcY = in.py;
                m->_dstY = in.py + TILE_HEIGHT;
                m->_changeState(m, _moving, MOVING);
                return {m->_srcX,m->_srcY,true,m->_movingBack};
            } else { //Move4
                m->_moveFrames = HORI_SPEED;
                m->_srcX = in.px;
                m->_dstX = in.px - TILE_WIDTH;
                m->_srcY = in.py;
                m->_dstY = in.py;
                m->_changeState(m, _moving, MOVING);
                return {m->_srcX,m->_srcY,true,m->_movingBack};
            };
        } else if(in.keys.Forw() && m->_canMoveForw(in.px)) {
            if(in.keys.keys.Move3 && m->_canMoveNear(in.py)) {
                m->_moveFrames = DIAG_SPEED;
                m->_srcX = in.px;
                m->_dstX = in.px + TILE_WIDTH;
                m->_srcY = in.py;
                m->_dstY = in.py - TILE_HEIGHT;
                m->_changeState(m, _moving, MOVING);
                return {m->_srcX,m->_srcY,true,m->_movingBack};
            } else if(in.keys.keys.Move9 && m->_canMoveFar(in.py)) {
                m->_moveFrames = DIAG_SPEED;
                m->_srcX = in.px;
                m->_dstX = in.px + TILE_WIDTH;
                m->_srcY = in.py;
                m->_dstY = in.py + TILE_HEIGHT;
                m->_changeState(m, _moving, MOVING);
                return {m->_srcX,m->_srcY,true,m->_movingBack};
            } else { //Move6
                m->_moveFrames = HORI_SPEED;
                m->_srcX = in.px;
                m->_dstX = in.px + TILE_WIDTH;
                m->_srcY = in.py;
                m->_dstY = in.py;
                m->_changeState(m,_moving, MOVING);
                return {m->_srcX,m->_srcY,true,m->_movingBack};
            };
        } else {
            if(in.keys.Near() && m->_canMoveNear(in.py)) {
                m->_moveFrames = VERT_SPEED;
                m->_srcX = in.px;
                m->_dstX = in.px;
                m->_srcY = in.py;
                m->_dstY = in.py - TILE_HEIGHT;
                m->_changeState(m, _moving, MOVING);
                return {m->_srcX,m->_srcY,true,m->_movingBack};
            } else if(in.keys.Far() && m->_canMoveFar(in.py)) {
                m->_moveFrames = VERT_SPEED;
                m->_srcX = in.px;
                m->_dstX = in.px;
                m->_srcY = in.py;
                m->_dstY = in.py + TILE_HEIGHT;
                m->_changeState(m, _moving, MOVING);
                return {m->_srcX,m->_srcY,true,m->_movingBack};
            }
        }
        //no move
        return {in.px,in.py,false,m->_movingBack};
    };
    const Out Machine::_moving(StateMachine<In,Out>* sm, const In& in) {
        Machine* m = static_cast<Machine*>(sm);
        uint8_t curFrame = m->_currentFrame();
        if(curFrame >= m->_moveFrames) {
            m->_movingBack = false;
            m->_changeState(m, _idle,IDLE);
            m->_srcX = m->_dstX;
            m->_srcY = m->_dstY;
            m->_dstX = m->_dstX;
            m->_dstY = m->_dstY;
            return {m->_dstX,m->_dstY,false,m->_movingBack};
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
        return {curX,curY,true,m->_movingBack};
    };
    size_t Machine::serialize(char* addr) {
        size_t offset = 0;
        State sEnum = (State) _currentEnum();
        memcpy(addr,&sEnum,sizeof(sEnum));
        offset+=sizeof(sEnum);

        uint8_t frame = _currentFrame();
        memcpy(addr+offset,&frame,sizeof(frame));
        offset+=sizeof(frame);

        memcpy(addr+offset, &_moveFrames,sizeof(_moveFrames));
        offset+=sizeof(_moveFrames);

        memcpy(addr+offset, &_srcX,sizeof(_srcX));
        offset+=sizeof(_srcX);

        memcpy(addr+offset, &_srcY,sizeof(_srcY));
        offset+=sizeof(_srcY);

        memcpy(addr+offset, &_dstX,sizeof(_dstX));
        offset+=sizeof(_dstX);

        memcpy(addr+offset, &_dstY,sizeof(_dstY));
        offset+=sizeof(_dstY);

        memcpy(addr+offset,&_movingBack,sizeof(_movingBack));
        offset+=sizeof(_movingBack);

        return offset;
    }

    Machine::Machine(char* addr, size_t& size) {
        State sEnum = *reinterpret_cast<State*>(addr+size);
        size += sizeof(sEnum);
        switch(sEnum) {
            case IDLE:
                _changeState(this,_idle,IDLE);
                break;
            case MOVING:
                _changeState(this,_moving,MOVING);
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
};