#include "ChatServer.h"

ChatServer::ChatServer() : host(""), port(""), name(""), con_count(0) {

                           };

ChatServer::ChatServer(const ChatServer &cs) : host(cs.host), port(cs.port), name(cs.name), con_count(cs.con_count) {

                                               };
