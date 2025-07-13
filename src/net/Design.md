# High Level Design

net is the library for implementing rollback and communicating over UDP. It does not involve any actual game logic. It expects things like game state and inputs to be entirely serializable. presentation will also be implemented in another library.

both players (only 2 max) are expected to keep identical copies of net. players will negotiate who is player 1 and player 2 beforehand. your copy of net will:

1. keep perfect track of your own inputs up to an arbitrary #
2. keep track of your opponents inputs received up to that #
3. pester the opponent for any missing inputs
4. desync if any missed frames go out of scope

there are several types of messages it can expect:

1. a most recent input - with frame # and serialized input
2. a request for a particular frame of input
3. a response to such a request, with frame #, serialized input, and the serialized game state the responder has for that frame

when processing a requested frame, if the game state matches what the responder mentioned we will assume any previous predictions were correct, and therefore not request earlier missing frames.