#include "game.h"

void loadSoundBgm(const string path, sf::Sound& sound, sf::SoundBuffer& buffer) { // 注意 sf::Sound&
    if (!buffer.loadFromFile(path)) {
        throw runtime_error("加载 BGM 失败:" + path);
    }
    sound.setBuffer(buffer);
    sound.setLoop(true);        //循环播放
}

void loadSoundClip(const string path, sf::Sound& sound, sf::SoundBuffer& buffer) { // 注意 sf::Sound&
    if (!buffer.loadFromFile(path)) {
        throw runtime_error("加载 CLIP 失败:" + path);
    }
    sound.setBuffer(buffer);
    sound.setLoop(false);
}
