#pragma once

#include <IGBScreenDrawable.h>
#include <SFML/Graphics.hpp>

class Renderer : public vrgb::IGBScreenDrawable
{
private:
	sf::RenderWindow	*_window;
	sf::Texture			*_texture;
	sf::Image			*_framebuffer;
	sf::Sprite			_sprite;
	sf::Text			_textFps;
	sf::Font			_font;

	const int	_WIDTH;
	const int	_HEIGHT;
public:
	Renderer(void);
	virtual ~Renderer(void);

	bool	Init();
	bool	IsOpen();
	void	DrawFps(float value);

	virtual void	OnPreDraw();
	virtual void	OnPostDraw();
	virtual void	OnDrawPixel(int idColor, int x, int y);
	virtual void	OnRefreshScreen();
	virtual void	OnClear();
};

