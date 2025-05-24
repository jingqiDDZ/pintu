#include "game.h"

sf::Sound loadSoundBgm(const string path) { // 注意 sf::Sound&
    sf::SoundBuffer buffer;
    sf::Sound sound;
    if (!buffer.loadFromFile(path)) {
        throw runtime_error("加载 BGM 失败: " + path);
    }
    sound.setBuffer(buffer);
    sound.setLoop(true);
    return sound;
}

sf::Sound loadSoundClip(const string path) { // 注意 sf::Sound&
    sf::SoundBuffer buffer;
    sf::Sound sound;
    if (!buffer.loadFromFile(path)) {
        throw runtime_error("加载 CLIP 失败: " + path);
    }
    sound.setBuffer(buffer);
    sound.setLoop(false);
    return sound;
}
