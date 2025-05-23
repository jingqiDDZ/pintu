#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
//zuihaol
int main()
{
    // 创建窗口
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML 音频播放示例");

    // 加载音频
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("./assets/audio/along.wav")) {  // 确保项目目录下有 sound.wav 文件
        return EXIT_FAILURE;
    }

    sf::Sound sound;
    sound.setBuffer(buffer);

    // 创建按钮
    sf::RectangleShape button(sf::Vector2f(200, 100));
    button.setPosition(300, 250);
    button.setFillColor(sf::Color::Green);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {  // 需要字体文件
        return EXIT_FAILURE;
    }

    sf::Text buttonText("播放音效", font, 30);
    buttonText.setPosition(330, 280);
    buttonText.setFillColor(sf::Color::Black);

    // 主循环
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            // 关闭窗口
            if (event.type == sf::Event::Closed)
                window.close();

            // 鼠标点击检测
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (button.getGlobalBounds().contains(mousePos.x, mousePos.y))
                    {
                        sound.play();  // 播放音效
                        button.setFillColor(sf::Color::Red);  // 按钮反馈
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonReleased)
            {
                button.setFillColor(sf::Color::Green);
            }
        }

        // 渲染
        window.clear(sf::Color::White);
        window.draw(button);
        window.draw(buttonText);
        window.display();
    }

    return 0;
}
