#include "GBRenderer.h"

using namespace tgb;

GBRenderer::GBRenderer()
{
}


GBRenderer::~GBRenderer()
{
}

void	GBRenderer::reset()
{
}

int		GBRenderer::check_pad()
{
	return 0;
}

word	GBRenderer::map_color(word gb_col)
{
	return gb_col;
}

word	GBRenderer::unmap_color(word gb_col)
{
	return gb_col;
}

byte	GBRenderer::get_time(int type)
{
	return 0;
}

void	GBRenderer::set_time(int type, byte dat)
{
}

word	GBRenderer::get_sensor(bool x_y)
{
	return 0;
}

void	GBRenderer::set_bibrate(bool bibrate)
{
}

void	GBRenderer::output_log(char *mes, ...)
{
}