#include "Renderer.h"
#include <string>
#include "Pad.h"

static const sf::Uint8 g_palettes[][4][3] =
{
	{
		{ 16,  57,  16},
		{ 49,  99,  49},
		{140, 173,  16},
		{156, 189,  16}
	},
	{
		{  0,   0,   0},
		{ 85,  85,  85},
		{170, 170, 170},
		{255, 255, 255}
	}
};

Renderer::Renderer(void)
:	_window(nullptr)
,	_texture(nullptr)
,	_framebuffer(nullptr)
,	_WIDTH(160)
,	_HEIGHT(144)
{
}


Renderer::~Renderer(void)
{
	delete _window;
	delete _texture;
	delete _framebuffer;
}

bool	Renderer::Init()
{
	_window = new sf::RenderWindow(sf::VideoMode(_WIDTH, _HEIGHT), "libvrgb test");
	if (_window == nullptr)
		return false;
	_texture = new sf::Texture();
	if (_texture == nullptr)
		return false;
	_framebuffer = new sf::Image();
	if (_framebuffer == nullptr)
		return false;
	_framebuffer->create(_WIDTH, _HEIGHT, sf::Color::Black);
	_font.loadFromFile("ComicBook.ttf");
	_textFps.setFont(_font);
	_textFps.setCharacterSize(10);
	_textFps.setColor(sf::Color::Red);
	vrgb::Pad::ResetStates();
	return true;
}

bool	Renderer::IsOpen()
{
	return _window->isOpen();
}

void	Renderer::DrawFps(float value)
{
	_textFps.setString("FPS:" + std::to_string(value));
}

void	Renderer::OnPreDraw()
{
}

void	Renderer::OnPostDraw()
{
}

void	Renderer::OnDrawPixel(int idColor, int x, int y)
{
	sf::Uint8	r = g_palettes[0][idColor][0];
	sf::Uint8	g = g_palettes[0][idColor][1];
	sf::Uint8	b = g_palettes[0][idColor][2];
	sf::Color	color(r,g,b);

	_framebuffer->setPixel(x, y, color);
}

void	Renderer::OnRefreshScreen()
{
	sf::Event event;
	while (_window->pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			_window->close();
		if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::Q: //A
				vrgb::Pad::SetState(vrgb::Pad::gbA, event.type == sf::Event::KeyPressed ? true : false);
				break;
			case sf::Keyboard::W: //B
				vrgb::Pad::SetState(vrgb::Pad::gbB, event.type == sf::Event::KeyPressed ? true : false);
				break;
			case sf::Keyboard::Left:
				vrgb::Pad::SetState(vrgb::Pad::gbLEFT, event.type == sf::Event::KeyPressed ? true : false);
				break;
			case sf::Keyboard::Right:
				vrgb::Pad::SetState(vrgb::Pad::gbRIGHT, event.type == sf::Event::KeyPressed ? true : false);
				break;
			case sf::Keyboard::Up:
				vrgb::Pad::SetState(vrgb::Pad::gbUP, event.type == sf::Event::KeyPressed ? true : false);
				break;
			case sf::Keyboard::Down:
				vrgb::Pad::SetState(vrgb::Pad::gbDOWN, event.type == sf::Event::KeyPressed ? true : false);
				break;
			case sf::Keyboard::D: //Select
				vrgb::Pad::SetState(vrgb::Pad::gbSTART, event.type == sf::Event::KeyPressed ? true : false);
				break;
			case sf::Keyboard::S: //Start
				vrgb::Pad::SetState(vrgb::Pad::gbSELECT, event.type == sf::Event::KeyPressed ? true : false);
				break;
			}
		}
	}

	_texture->loadFromImage(*_framebuffer);
	_sprite.setTexture(*_texture);
	_window->draw(_sprite);
	_window->draw(_textFps);
	_window->display();
}

void	Renderer::OnClear()
{
	 _window->clear();
}