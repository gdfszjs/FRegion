/* 
	load all textures
*/

#pragma once
#include "CommonGL.h"
#include "Common.h"

class TextureLoader
{
	public:
		~TextureLoader() 
		{
			if(textureids != 0)
				delete [] textureids;
		}
	
		static TextureLoader * Instance()
		{
			if(instance == 0)
			{
				instance = new TextureLoader();
				static Cleaner clr;
			} 
			return instance;
		}
		
		void loadTexture(char * filepath,int id);
		GLuint getTextureId(int id);
		
	private:
		static TextureLoader * instance;
		
		GLuint * textureids; // at most 8 
		
		TextureLoader()
		{
			textureids = 0;
		}
		// automatically delete the singleton
		class Cleaner
		{
		public:
			Cleaner() {}
			~Cleaner()
			{
				if(TextureLoader::Instance())
					delete TextureLoader::Instance();
			}
		};
};