#include "includes.hpp"
#include <thread>

int main()
{
	driver::process_id = driver::find_process(_("FortniteClient-Win64-Shipping.exe"));
	if (driver::process_id != 0)
	{
		if (driver::find_driver())
		{
			driver::image_base = driver::find_image();
			Input::Init();
			create_overlay();
			directx_init();
			render_loop();
		}
	}
	return 0;
}