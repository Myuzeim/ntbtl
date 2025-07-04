#include "move_machine.h"

namespace MoveMachine {
    Machine::Machine() {
        _movingBack = false;
        _changeState(this,_idle);
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
                m->_changeState(m, _moving);
                return {m->_srcX,m->_srcY,true,m->_movingBack};
            } else if(in.keys.keys.Move7 && m->_canMoveFar(in.py)) {
                m->_moveFrames = DIAG_SPEED;
                m->_srcX = in.px;
                m->_dstX = in.px - TILE_WIDTH;
                m->_srcY = in.py;
                m->_dstY = in.py + TILE_HEIGHT;
                m->_changeState(m, _moving);
                return {m->_srcX,m->_srcY,true,m->_movingBack};
            } else { //Move4
                m->_moveFrames = HORI_SPEED;
                m->_srcX = in.px;
                m->_dstX = in.px - TILE_WIDTH;
                m->_srcY = in.py;
                m->_dstY = in.py;
                m->_changeState(m, _moving);
                return {m->_srcX,m->_srcY,true,m->_movingBack};
            };
        } else if(in.keys.Forw() && m->_canMoveForw(in.px)) {
            if(in.keys.keys.Move3 && m->_canMoveNear(in.py)) {
                m->_moveFrames = DIAG_SPEED;
                m->_srcX = in.px;
                m->_dstX = in.px + TILE_WIDTH;
                m->_srcY = in.py;
                m->_dstY = in.py - TILE_HEIGHT;
                m->_changeState(m, _moving);
                return {m->_srcX,m->_srcY,true,m->_movingBack};
            } else if(in.keys.keys.Move9 && m->_canMoveFar(in.py)) {
                m->_moveFrames = DIAG_SPEED;
                m->_srcX = in.px;
                m->_dstX = in.px + TILE_WIDTH;
                m->_srcY = in.py;
                m->_dstY = in.py + TILE_HEIGHT;
                m->_changeState(m, _moving);
                return {m->_srcX,m->_srcY,true,m->_movingBack};
            } else { //Move6
                m->_moveFrames = HORI_SPEED;
                m->_srcX = in.px;
                m->_dstX = in.px + TILE_WIDTH;
                m->_srcY = in.py;
                m->_dstY = in.py;
                m->_changeState(m,_moving);
                return {m->_srcX,m->_srcY,true,m->_movingBack};
            };
        } else {
            if(in.keys.Near() && m->_canMoveNear(in.py)) {
                m->_moveFrames = VERT_SPEED;
                m->_srcX = in.px;
                m->_dstX = in.px;
                m->_srcY = in.py;
                m->_dstY = in.py - TILE_HEIGHT;
                m->_changeState(m, _moving);
                return {m->_srcX,m->_srcY,true,m->_movingBack};
            } else if(in.keys.Far() && m->_canMoveFar(in.py)) {
                m->_moveFrames = VERT_SPEED;
                m->_srcX = in.px;
                m->_dstX = in.px;
                m->_srcY = in.py;
                m->_dstY = in.py + TILE_HEIGHT;
                m->_changeState(m, _moving);
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
            m->_changeState(m, _idle);
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
};