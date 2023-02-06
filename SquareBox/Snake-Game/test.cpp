//#include <iostream>
//#include <vector>
//#include <cmath>
//#include <string>
//#include <sstream>
//#include <fstream>
//#include <SquareBox-Core/SquareBoxGameEngine.h>
//#include "Snake.h"
//#include "Wave.h"
//#include "GameObjects.h"
//#include "GameWorld.h"
//#include "GameAction.h"
//#include "PlayerInputs.h"
//#include "ReplayInputs.h"
//
//
//
//std::vector<WavePoint>* Wave::surround = NULL;
//
//int main(int argc, const char* argv[])
//{
//
//
//
//
//    const std::string str_blur_h = "uniform sampler2D image;uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);void main(){vec2 TexCoords = gl_TexCoord[0].xy; vec2 tex_offset = 1.0 / textureSize(image, 0); vec3 result = texture(image, TexCoords).rgb * weight[0]; for(int i = 1; i < 5; ++i){result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];}gl_FragColor = vec4(result, 1.0);}";
//    const std::string str_blur_v = "uniform sampler2D image;uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);void main(){vec2 TexCoords = gl_TexCoord[0].xy; vec2 tex_offset = 1.0 / textureSize(image, 0); vec3 result = texture(image, TexCoords).rgb * weight[0]; for(int i = 1; i < 5; ++i){result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];}gl_FragColor = vec4(result, 1.0);}";
//   
//    sf::Shader blur_h, blur_v;
//    blur_h.loadFromMemory(str_blur_h, sf::Shader::Fragment);
//    blur_v.loadFromMemory(str_blur_v, sf::Shader::Fragment);
//
//    //soundManager.play("ambience");
//
//
//
//
//    
// 
//
//
//
//
//
//    while (window.isOpen())
//    {
//      
//
//
//  
//  
//    }
//
//    std::ofstream file("replay");
//    file << replay.size() << "\n";
//    for (GameAction& ga : replay)
//    {
//        file << ga.tick << " " << ga.type << " " << ga.x << " " << ga.y << " " << ga.vx << " " << ga.vy << " " << ga.timer << "\n";
//    }
//    file.close();
//
//    return 0;
//}
