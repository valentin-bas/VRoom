#pragma once

#include "gb_core\renderer.h"

#ifdef LIBTGB_EXPORT
#define LIBTGB_API __declspec(dllexport)
#else
#define LIBTGB_API __declspec(dllimport)
#endif

namespace tgb
{
	class LIBTGB_API GBRenderer : public renderer
	{
	public:
		GBRenderer();
		virtual ~GBRenderer();


	public:
		virtual void	reset() override;
		virtual word	map_color(word gb_col) override;
		virtual word	unmap_color(word gb_col) override;

		virtual byte	get_time(int type) override;
		virtual void	set_time(int type, byte dat) override;
		virtual word	get_sensor(bool x_y) override;
		virtual void	set_bibrate(bool bibrate) override;
		virtual void	output_log(char *mes, ...) override;
	};
}
