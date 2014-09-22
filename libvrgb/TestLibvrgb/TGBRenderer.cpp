#include "TGBRenderer.h"
#include <string>

TGBRenderer::TGBRenderer(void)
	: _window(nullptr)
	, _texture(nullptr)
	, _framebuffer(nullptr)
	, _WIDTH(160)
	, _HEIGHT(144)
{
}


TGBRenderer::~TGBRenderer(void)
{
	delete _window;
	delete _texture;
	delete _framebuffer;
}

bool	TGBRenderer::Init()
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
	//vrgb::Pad::ResetStates();
	return true;
}

bool	TGBRenderer::IsOpen()
{
	return _window->isOpen();
}

void	TGBRenderer::DrawFps(float value)
{
	_textFps.setString("FPS:" + std::to_string(value));
}

bool	TGBRenderer::Update()
{
	return true;
}

void	TGBRenderer::refresh()
{
	sf::Event event;
	while (_window->pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			_window->close();
	}

	_texture->loadFromImage(*_framebuffer);
	_sprite.setTexture(*_texture);
	_window->draw(_sprite);
	_window->draw(_textFps);
	_window->display();
}

void	TGBRenderer::render_screen(byte *buf, int width, int height, int depth)
{
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			sf::Uint16	gb_col = *buf++;
			sf::Uint8	r = ((gb_col >> 0) & 0x1f) << 3;
			sf::Uint8	g = ((gb_col >> 5) & 0x1f) << 3;
			sf::Uint8	b = ((gb_col >> 10) & 0x1f) << 3;

			sf::Color	color(r, g, b);

			_framebuffer->setPixel(j, i, color);
		}
	}
}

/*void	Renderer::OnPreDraw()
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
	sf::Color	color(r, g, b);

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
}*/