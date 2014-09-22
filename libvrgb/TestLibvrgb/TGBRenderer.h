#pragma once

#include <GBRenderer.h>
#include <SFML/Graphics.hpp>

class TGBRenderer : public tgb::GBRenderer
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
	TGBRenderer(void);
	virtual ~TGBRenderer(void);

	bool	Init();
	bool	Update();
	bool	IsOpen();
	void	DrawFps(float value);

	virtual void	refresh() override;
	virtual void	render_screen(byte *buf, int width, int height, int depth) override;
};

