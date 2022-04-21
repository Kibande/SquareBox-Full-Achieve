#pragma once
#ifdef SQB_PLATFORM_ANDROID
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#else
#include<GL/glew.h>
#endif
#include <SquareBox-Logger/Logger.h>
#include <glm/glm.hpp>
namespace SquareBox {
	/*
	Colour Codes Adapted from
	https://www.rapidtables.com/web/color/RGB_Color.html
	*/

	enum class Color {
		maroon,
		dark_red,
		brown,
		firebrick,
		crimson,
		red,
		tomato,
		coral,
		indian_red,
		light_coral,
		dark_salmon,
		salmon,
		light_salmon,
		orange_red,
		dark_orange,
		orange,
		gold,
		dark_golden_rod,
		golden_rod,
		pale_golden_rod,
		dark_khaki,
		khaki,
		olive,
		yellow,
		yellow_green,
		dark_olive_green,
		olive_drab,
		lawn_green,
		chart_reuse,
		green_yellow,
		dark_green,
		green,
		forest_green,
		lime,
		lime_green,
		light_green,
		pale_green,
		dark_sea_green,
		medium_spring_green,
		spring_green,
		sea_green,
		medium_aqua_marine,
		medium_sea_green,
		light_sea_green,
		dark_slate_gray,
		teal,
		dark_cyan,
		aqua,
		cyan,
		light_cyan,
		dark_turquoise,
		turquoise,
		medium_turquoise,
		pale_turquoise,
		aqua_marine,
		powder_blue,
		cadet_blue,
		steel_blue,
		corn_flower_blue,
		deep_sky_blue,
		dodger_blue,
		light_blue,
		sky_blue,
		light_sky_blue,
		midnight_blue,
		navy,
		dark_blue,
		medium_blue,
		blue,
		royal_blue,
		blue_violet,
		indigo,
		dark_slate_blue,
		slate_blue,
		medium_slate_blue,
		medium_purple,
		dark_magenta,
		dark_violet,
		dark_orchid,
		medium_orchid,
		purple,
		thistle,
		plum,
		violet,
		magenta,
		fushsia,
		orchid,
		medium_violet_red,
		pale_violet_red,
		deep_pink,
		hot_pink,
		light_pink,
		pink,
		antique_white,
		beige,
		bisque,
		blanched_almond,
		wheat,
		corn_silk,
		lemon_chiffon,
		light_golden_rod_yellow,
		light_yellow,
		saddle_brown,
		sienna,
		chocolate,
		peru,
		sandy_brown,
		burly_wood,
		tan,
		rosy_brown,
		moccasin,
		navajo_white,
		peach_puff,
		misty_rose,
		lavender_blush,
		linen,
		old_lace,
		papaya_whip,
		sea_shell,
		mint_cream,
		slate_gray,
		light_slate_gray,
		light_steel_blue,
		lavender,
		floral_white,
		alice_blue,
		ghost_white,
		honeydew,
		ivory,
		azure,
		snow,
		black,
		dim_grey,
		grey,
		dark_grey,
		sliver,
		light_grey,
		gainsboro,
		white_smoke,
		white
	};

	namespace RenderEngine {
		class ColorRGBA8 {
		public:
			ColorRGBA8() :r(0), g(0), b(0), a(0) {
			}
			ColorRGBA8(unsigned int R, unsigned int G, unsigned int B, unsigned int A) :r(R), g(G), b(B), a(A) {
			}
			ColorRGBA8(glm::ivec4 color_in_vec4_format_) {
				r = color_in_vec4_format_.x;
				g = color_in_vec4_format_.y;
				b = color_in_vec4_format_.z;
				a = color_in_vec4_format_.w;
			}
			ColorRGBA8(SquareBox::Color color_) {
				bool isColorPresent = true;

				if (color_ == SquareBox::Color::maroon) {
					r = 128;
					g = 0;
					b = 0;
					a = 255;
				}
				else if (color_ == SquareBox::Color::dark_red) {
					r = 138;
					g = 0;
					b = 0;
					a = 255;
				}
				else if (color_ == SquareBox::Color::brown) {
					r = 165;
					g = 42;
					b = 42;
					a = 255;
				}
				else if (color_ == SquareBox::Color::firebrick) {
					r = 178;
					g = 34;
					b = 34;
					a = 255;
				}
				else if (color_ == SquareBox::Color::crimson) {
					r = 220;
					g = 20;
					b = 60;
					a = 255;
				}
				else if (color_ == SquareBox::Color::red) {
					r = 255;
					g = 0;
					b = 0;
					a = 255;
				}
				else if (color_ == SquareBox::Color::tomato) {
					r = 255;
					g = 99;
					b = 71;
					a = 255;
				}
				else if (color_ == SquareBox::Color::coral) {
					r = 255;
					g = 127;
					b = 80;
					a = 255;
				}
				else if (color_ == SquareBox::Color::indian_red) {
					r = 205;
					g = 92;
					b = 92;
					a = 255;
				}
				else if (color_ == SquareBox::Color::light_coral) {
					r = 240;
					g = 128;
					b = 128;
					a = 255;
				}
				else if (color_ == SquareBox::Color::dark_salmon) {
					r = 233;
					g = 150;
					b = 122;
					a = 255;
				}
				else if (color_ == SquareBox::Color::salmon) {
					r = 250;
					g = 128;
					b = 114;
					a = 255;
				}
				else if (color_ == SquareBox::Color::light_salmon) {
					r = 255;
					g = 160;
					b = 122;
					a = 255;
				}
				else if (color_ == SquareBox::Color::orange_red) {
					r = 255;
					g = 69;
					b = 0;
					a = 255;
				}
				else if (color_ == SquareBox::Color::dark_orange) {
					r = 255;
					g = 140;
					b = 0;
					a = 255;
				}
				else if (color_ == SquareBox::Color::orange) {
					r = 255;
					g = 165;
					b = 0;
					a = 255;
				}
				else if (color_ == SquareBox::Color::gold) {
					r = 255;
					g = 215;
					b = 0;
					a = 255;
				}
				else if (color_ == SquareBox::Color::dark_golden_rod) {
					r = 184;
					g = 134;
					b = 11;
					a = 255;
				}
				else if (color_ == SquareBox::Color::golden_rod) {
					r = 218;
					g = 165;
					b = 32;
					a = 255;
				}
				else if (color_ == SquareBox::Color::pale_golden_rod) {
					r = 238;
					g = 232;
					b = 170;
					a = 255;
				}
				else if (color_ == SquareBox::Color::dark_khaki) {
					r = 189;
					g = 183;
					b = 107;
					a = 255;
				}
				else if (color_ == SquareBox::Color::khaki) {
					r = 240;
					g = 230;
					b = 140;
					a = 255;
				}
				else if (color_ == SquareBox::Color::olive) {
					r = 128;
					g = 128;
					b = 0;
					a = 255;
				}
				else if (color_ == SquareBox::Color::yellow) {
					r = 255;
					g = 255;
					b = 0;
					a = 255;
				}
				else if (color_ == SquareBox::Color::yellow_green) {
					r = 154;
					g = 205;
					b = 50;
					a = 255;
				}
				else if (color_ == SquareBox::Color::dark_olive_green) {
					r = 85;
					g = 107;
					b = 47;
					a = 255;
				}
				else if (color_ == SquareBox::Color::olive_drab) {
					r = 107;
					g = 142;
					b = 35;
					a = 255;
				}
				else if (color_ == SquareBox::Color::lawn_green) {
					r = 124;
					g = 252;
					b = 0;
					a = 255;
				}
				else if (color_ == SquareBox::Color::chart_reuse) {
					r = 127;
					g = 255;
					b = 0;
					a = 255;
				}
				else if (color_ == SquareBox::Color::green_yellow) {
					r = 173;
					g = 255;
					b = 47;
					a = 255;
				}
				else if (color_ == SquareBox::Color::dark_green) {
					r = 0;
					g = 100;
					b = 0;
					a = 255;
				}
				else if (color_ == SquareBox::Color::green) {
					r = 0;
					g = 128;
					b = 0;
					a = 255;
				}
				else if (color_ == SquareBox::Color::forest_green) {
					r = 34;
					g = 139;
					b = 34;
					a = 255;
				}
				else if (color_ == SquareBox::Color::lime) {
					r = 0;
					g = 255;
					b = 0;
					a = 255;
				}
				else if (color_ == SquareBox::Color::lime_green) {
					r = 50;
					g = 205;
					b = 50;
					a = 255;
				}
				else if (color_ == SquareBox::Color::light_green) {
					r = 144;
					g = 238;
					b = 144;
					a = 255;
				}
				else if (color_ == SquareBox::Color::pale_green) {
					r = 152;
					g = 251;
					b = 152;
					a = 255;
				}
				else if (color_ == SquareBox::Color::dark_sea_green) {
					r = 143;
					g = 188;
					b = 143;
					a = 255;
				}
				else if (color_ == SquareBox::Color::medium_spring_green) {
					r = 0;
					g = 250;
					b = 154;
					a = 255;
				}
				else if (color_ == SquareBox::Color::spring_green) {
					r = 0;
					g = 255;
					b = 127;
					a = 255;
				}
				else if (color_ == SquareBox::Color::sea_green) {
					r = 46;
					g = 139;
					b = 87;
					a = 255;
				}
				else if (color_ == SquareBox::Color::medium_aqua_marine) {
					r = 102;
					g = 205;
					b = 170;
					a = 255;
				}
				else if (color_ == SquareBox::Color::medium_sea_green) {
					r = 60;
					g = 179;
					b = 113;
					a = 255;
				}
				else if (color_ == SquareBox::Color::light_sea_green) {
					r = 32;
					g = 178;
					b = 170;
					a = 255;
				}
				else if (color_ == SquareBox::Color::dark_slate_gray) {
					r = 47;
					g = 79;
					b = 79;
					a = 255;
				}
				else if (color_ == SquareBox::Color::teal) {
					r = 0;
					g = 128;
					b = 128;
					a = 255;
				}
				else if (color_ == SquareBox::Color::dark_cyan) {
					r = 0;
					g = 139;
					b = 139;
					a = 255;
				}
				else if (color_ == SquareBox::Color::aqua) {
					r = 0;
					g = 255;
					b = 255;
					a = 255;
				}
				else if (color_ == SquareBox::Color::cyan) {
					r = 0;
					g = 255;
					b = 255;
					a = 255;
				}
				else if (color_ == SquareBox::Color::light_cyan) {
					r = 224;
					g = 255;
					b = 255;
					a = 255;
				}
				else if (color_ == SquareBox::Color::dark_turquoise) {
					r = 0;
					g = 206;
					b = 209;
					a = 255;
				}
				else if (color_ == SquareBox::Color::turquoise) {
					r = 64;
					g = 244;
					b = 208;
					a = 255;
				}
				else if (color_ == SquareBox::Color::medium_turquoise) {
					r = 72;
					g = 209;
					b = 204;
					a = 255;
				}
				else if (color_ == SquareBox::Color::pale_turquoise) {
					r = 175;
					g = 238;
					b = 238;
					a = 255;
				}
				else if (color_ == SquareBox::Color::aqua_marine) {
					r = 127;
					g = 255;
					b = 212;
					a = 255;
				}
				else if (color_ == SquareBox::Color::powder_blue) {
					r = 176;
					g = 224;
					b = 230;
					a = 255;
				}
				else if (color_ == SquareBox::Color::cadet_blue) {
					r = 95;
					g = 158;
					b = 160;
					a = 255;
				}
				else if (color_ == SquareBox::Color::steel_blue) {
					r = 70;
					g = 130;
					b = 180;
					a = 255;
				}
				else if (color_ == SquareBox::Color::corn_flower_blue) {
					r = 100;
					g = 149;
					b = 237;
					a = 255;
				}
				else if (color_ == SquareBox::Color::deep_sky_blue) {
					r = 0;
					g = 191;
					b = 255;
					a = 255;
				}
				else if (color_ == SquareBox::Color::dodger_blue) {
					r = 30;
					g = 144;
					b = 255;
					a = 255;
				}
				else if (color_ == SquareBox::Color::light_blue) {
					r = 173;
					g = 216;
					b = 230;
					a = 255;
				}
				else if (color_ == SquareBox::Color::sky_blue) {
					r = 135;
					g = 206;
					b = 235;
					a = 255;
				}
				else if (color_ == SquareBox::Color::light_sky_blue) {
					r = 135;
					g = 206;
					b = 250;
					a = 255;
				}
				else if (color_ == SquareBox::Color::midnight_blue) {
					r = 25;
					g = 25;
					b = 112;
					a = 255;
				}
				else if (color_ == SquareBox::Color::navy) {
					r = 0;
					g = 0;
					b = 128;
					a = 255;
				}
				else if (color_ == SquareBox::Color::dark_blue) {
					r = 0;
					g = 0;
					b = 139;
					a = 255;
				}
				else if (color_ == SquareBox::Color::medium_blue) {
					r = 0;
					g = 0;
					b = 205;
					a = 255;
				}
				else if (color_ == SquareBox::Color::blue) {
					r = 0;
					g = 0;
					b = 255;
					a = 255;
				}
				else if (color_ == SquareBox::Color::royal_blue) {
					r = 65;
					g = 105;
					b = 255;
					a = 255;
				}
				else if (color_ == SquareBox::Color::blue_violet) {
					r = 138;
					g = 43;
					b = 226;
					a = 255;
				}
				else if (color_ == SquareBox::Color::indigo) {
					r = 75;
					g = 0;
					b = 130;
					a = 255;
				}
				else if (color_ == SquareBox::Color::dark_slate_blue) {
					r = 72;
					g = 61;
					b = 139;
					a = 255;
				}
				else if (color_ == SquareBox::Color::slate_blue) {
					r = 106;
					g = 90;
					b = 205;
					a = 255;
				}
				else if (color_ == SquareBox::Color::medium_slate_blue) {
					r = 123;
					g = 104;
					b = 238;
					a = 255;
				}
				else if (color_ == SquareBox::Color::medium_purple) {
					r = 147;
					g = 112;
					b = 219;
					a = 255;
				}
				else if (color_ == SquareBox::Color::dark_magenta) {
					r = 139;
					g = 0;
					b = 139;
					a = 255;
				}
				else if (color_ == SquareBox::Color::dark_violet) {
					r = 148;
					g = 0;
					b = 211;
					a = 255;
				}
				else if (color_ == SquareBox::Color::dark_orchid) {
					r = 153;
					g = 50;
					b = 204;
					a = 255;
				}
				else if (color_ == SquareBox::Color::medium_orchid) {
					r = 186;
					g = 85;
					b = 211;
					a = 255;
				}
				else if (color_ == SquareBox::Color::purple) {
					r = 128;
					g = 0;
					b = 128;
					a = 255;
				}
				else if (color_ == SquareBox::Color::thistle) {
					r = 216;
					g = 191;
					b = 216;
					a = 255;
				}
				else if (color_ == SquareBox::Color::plum) {
					r = 221;
					g = 160;
					b = 221;
					a = 255;
				}
				else if (color_ == SquareBox::Color::violet) {
					r = 238;
					g = 130;
					b = 238;
					a = 255;
				}
				else if (color_ == SquareBox::Color::magenta) {
					r = 255;
					g = 0;
					b = 255;
					a = 255;
				}
				else if (color_ == SquareBox::Color::fushsia) {
					r = 255;
					g = 0;
					b = 255;
					a = 255;
				}
				else if (color_ == SquareBox::Color::orchid) {
					r = 218;
					g = 112;
					b = 214;
					a = 255;
				}
				else if (color_ == SquareBox::Color::medium_violet_red) {
					r = 199;
					g = 21;
					b = 133;
					a = 255;
				}
				else if (color_ == SquareBox::Color::pale_violet_red) {
					r = 219;
					g = 112;
					b = 147;
					a = 255;
				}
				else if (color_ == SquareBox::Color::deep_pink) {
					r = 255;
					g = 20;
					b = 147;
					a = 255;
				}
				else if (color_ == SquareBox::Color::hot_pink) {
					r = 255;
					g = 105;
					b = 180;
					a = 255;
				}
				else if (color_ == SquareBox::Color::light_pink) {
					r = 255;
					g = 182;
					b = 193;
					a = 255;
				}
				else if (color_ == SquareBox::Color::pink) {
					r = 255;
					g = 192;
					b = 203;
					a = 255;
				}
				else if (color_ == SquareBox::Color::antique_white) {
					r = 250;
					g = 235;
					b = 215;
					a = 255;
				}
				else if (color_ == SquareBox::Color::beige) {
					r = 245;
					g = 245;
					b = 220;
					a = 255;
				}
				else if (color_ == SquareBox::Color::bisque) {
					r = 255;
					g = 228;
					b = 220;
					a = 255;
				}
				else if (color_ == SquareBox::Color::blanched_almond) {
					r = 255;
					g = 235;
					b = 205;
					a = 255;
				}
				else if (color_ == SquareBox::Color::wheat) {
					r = 245;
					g = 222;
					b = 179;
					a = 255;
				}
				else if (color_ == SquareBox::Color::corn_silk) {
					r = 255;
					g = 248;
					b = 220;
					a = 255;
				}
				else if (color_ == SquareBox::Color::lemon_chiffon) {
					r = 250;
					g = 250;
					b = 205;
					a = 255;
				}
				else if (color_ == SquareBox::Color::light_golden_rod_yellow) {
					r = 250;
					g = 250;
					b = 210;
					a = 255;
				}
				else if (color_ == SquareBox::Color::light_yellow) {
					r = 255;
					g = 255;
					b = 224;
					a = 255;
				}
				else if (color_ == SquareBox::Color::saddle_brown) {
					r = 139;
					g = 69;
					b = 19;
					a = 255;
				}
				else if (color_ == SquareBox::Color::sienna) {
					r = 160;
					g = 82;
					b = 45;
					a = 255;
				}
				else if (color_ == SquareBox::Color::chocolate) {
					r = 210;
					g = 105;
					b = 30;
					a = 255;
				}
				else if (color_ == SquareBox::Color::sandy_brown) {
					r = 244;
					g = 164;
					b = 96;
					a = 255;
				}
				else if (color_ == SquareBox::Color::burly_wood) {
					r = 222;
					g = 184;
					b = 135;
					a = 255;
				}
				else if (color_ == SquareBox::Color::tan) {
					r = 210;
					g = 180;
					b = 140;
					a = 255;
				}
				else if (color_ == SquareBox::Color::rosy_brown) {
					r = 188;
					g = 143;
					b = 143;
					a = 255;
				}
				else if (color_ == SquareBox::Color::moccasin) {
					r = 255;
					g = 228;
					b = 181;
					a = 255;
				}
				else if (color_ == SquareBox::Color::navajo_white) {
					r = 255;
					g = 222;
					b = 173;
					a = 255;
				}
				else if (color_ == SquareBox::Color::peach_puff) {
					r = 255;
					g = 218;
					b = 185;
					a = 255;
				}
				else if (color_ == SquareBox::Color::misty_rose) {
					r = 255;
					g = 228;
					b = 225;
					a = 255;
				}
				else if (color_ == SquareBox::Color::lavender_blush) {
					r = 255;
					g = 240;
					b = 245;
					a = 255;
				}
				else if (color_ == SquareBox::Color::linen) {
					r = 250;
					g = 240;
					b = 230;
					a = 255;
				}
				else if (color_ == SquareBox::Color::old_lace) {
					r = 253;
					g = 245;
					b = 230;
					a = 255;
				}
				else if (color_ == SquareBox::Color::papaya_whip) {
					r = 255;
					g = 239;
					b = 213;
					a = 255;
				}
				else if (color_ == SquareBox::Color::sea_shell) {
					r = 255;
					g = 245;
					b = 238;
					a = 255;
				}
				else if (color_ == SquareBox::Color::mint_cream) {
					r = 245;
					g = 255;
					b = 250;
					a = 255;
				}
				else if (color_ == SquareBox::Color::slate_gray) {
					r = 112;
					g = 128;
					b = 144;
					a = 255;
				}
				else if (color_ == SquareBox::Color::light_slate_gray) {
					r = 119;
					g = 136;
					b = 153;
					a = 255;
				}
				else if (color_ == SquareBox::Color::light_steel_blue) {
					r = 176;
					g = 196;
					b = 222;
					a = 255;
				}
				else {
					isColorPresent = false;
				}
				if (color_ == SquareBox::Color::lavender) {
					r = 230;
					g = 230;
					b = 250;
					a = 255;
				}
				else if (color_ == SquareBox::Color::floral_white) {
					r = 255;
					g = 250;
					b = 240;
					a = 255;
				}
				else if (color_ == SquareBox::Color::alice_blue) {
					r = 240;
					g = 248;
					b = 255;
					a = 255;
				}
				else if (color_ == SquareBox::Color::ghost_white) {
					r = 248;
					g = 248;
					b = 255;
					a = 255;
				}
				else if (color_ == SquareBox::Color::honeydew) {
					r = 240;
					g = 255;
					b = 240;
					a = 255;
				}
				else if (color_ == SquareBox::Color::ivory) {
					r = 255;
					g = 255;
					b = 240;
					a = 255;
				}
				else if (color_ == SquareBox::Color::azure) {
					r = 240;
					g = 255;
					b = 255;
					a = 255;
				}
				else if (color_ == SquareBox::Color::snow) {
					r = 255;
					g = 250;
					b = 250;
					a = 255;
				}
				else if (color_ == SquareBox::Color::black) {
					r = 0;
					g = 0;
					b = 0;
					a = 255;
				}
				else if (color_ == SquareBox::Color::dim_grey) {
					r = 105;
					g = 105;
					b = 105;
					a = 255;
				}
				else if (color_ == SquareBox::Color::grey) {
					r = 128;
					g = 128;
					b = 128;
					a = 255;
				}
				else if (color_ == SquareBox::Color::dark_grey) {
					r = 169;
					g = 169;
					b = 169;
					a = 255;
				}
				else if (color_ == SquareBox::Color::sliver) {
					r = 192;
					g = 192;
					b = 192;
					a = 255;
				}
				else if (color_ == SquareBox::Color::light_grey) {
					r = 211;
					g = 211;
					b = 211;
					a = 255;
				}
				else if (color_ == SquareBox::Color::gainsboro) {
					r = 220;
					g = 220;
					b = 220;
					a = 255;
				}
				else if (color_ == SquareBox::Color::white_smoke) {
					r = 245;
					g = 245;
					b = 245;
					a = 255;
				}
				else if (color_ == SquareBox::Color::white) {
					r = 255;
					g = 255;
					b = 255;
					a = 255;
				}
				else {
					if (!isColorPresent) {
						SBX_CORE_ERROR("Undefined Color");
					}
				}
			}

			GLubyte r;
			GLubyte g;
			GLubyte b;
			GLubyte a;

			glm::ivec4 getVec4() { return  glm::ivec4(r,g,b,a); }
		};
	}
}
