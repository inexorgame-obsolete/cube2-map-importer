#pragma once

namespace inexor {
namespace cube2_map_importer {

	// 
	enum 
	{ 
		VSLOT_SHPARAM = 0, 
		VSLOT_SCALE, 
		VSLOT_ROTATION, 
		VSLOT_OFFSET, 
		VSLOT_SCROLL, 
		VSLOT_LAYER, 
		VSLOT_ALPHA,
		VSLOT_COLOR,
		VSLOT_NUM 
	};


	// 
	struct ShaderParam
	{
		ShaderParam()
		{
			name = "";
			type = 0;
			index = 0;
			loc = 0;
			val[0] = val[1] = val[2] = val[3] = 0;
		}

		//const char *name;
		
		std::string name;
		int type, index, loc;
		float val[4];
	};


	// 
	enum
	{
		SHPARAM_LOOKUP = 0,
		SHPARAM_VERTEX,
		SHPARAM_PIXEL,
		SHPARAM_UNIFORM
	};


	struct Slot;

	#define uint unsigned int


	struct VSlot
	{
		Slot *slot;
		VSlot *next;
		int index, changed;
		std::vector<ShaderParam> params;
		bool linked;
		float scale;
		int rotation, xoffset, yoffset;
		float scrollS, scrollT;
		int layer;
		float alphafront, alphaback;
		vec colorscale;
		vec glowcolor, pulseglowcolor;
		float pulseglowspeed;
		vec envscale;
		int skipped;

		VSlot(Slot *slot = NULL, int index = -1) : slot(slot), next(NULL), index(index), changed(0), skipped(0) 
		{ 
			reset();
			if(slot) addvariant(slot); 
		}

		void addvariant(Slot *slot);


		void reset()
		{
			params.clear();
			linked = false;
			scale = 1;
			rotation = xoffset = yoffset = 0;
			scrollS = scrollT = 0;
			layer = 0;
			alphafront = 0.5f;
			alphaback = 0;
			colorscale = vec(1, 1, 1);
			glowcolor = vec(1, 1, 1);
			pulseglowcolor = vec(0, 0, 0);
			pulseglowspeed = 0;
			envscale = vec(0, 0, 0);
		}

		void cleanup()
		{
			linked = false;
		}
	};



	typedef unsigned int GLuint;
	typedef unsigned int GLenum;
	#define uchar unsigned char

	struct Texture
	{
		enum
		{
			IMAGE      = 0,
			CUBEMAP    = 1,
			TYPE       = 0xFF,
        
			STUB       = 1<<8,
			TRANSIENT  = 1<<9,
			COMPRESSED = 1<<10, 
			ALPHA      = 1<<11,
			FLAGS      = 0xFF00
		};

		char *name;
		int type, w, h, xs, ys, bpp, clamp;
		bool mipmap, canreduce;
		GLuint id;
		uchar *alphamask;

		Texture() : alphamask(NULL) {}
	};

	#define DELETEP(p) if(p) { delete   p; p = 0; }
	#define DELETEA(p) if(p) { delete[] p; p = 0; }


	struct ImageData
	{
		int w, h, bpp, levels, align, pitch;
		GLenum compressed;
		uchar *data;
		void *owner;
		void (*freefunc)(void *);

		ImageData()
			: data(NULL), owner(NULL), freefunc(NULL)
		{}

    
		#define GL_FALSE 0

		ImageData(int nw, int nh, int nbpp, int nlevels = 1, int nalign = 0, GLenum ncompressed = GL_FALSE) 
		{ 
			setdata(NULL, nw, nh, nbpp, nlevels, nalign, ncompressed); 
		}

		ImageData(int nw, int nh, int nbpp, uchar *data)
			: owner(NULL), freefunc(NULL)
		{ 
			setdata(data, nw, nh, nbpp); 
		}

		//ImageData(SDL_Surface *s) { wrap(s); }
		~ImageData() { cleanup(); }

		void setdata(uchar *ndata, int nw, int nh, int nbpp, int nlevels = 1, int nalign = 0, GLenum ncompressed = GL_FALSE)
		{
			w = nw;
			h = nh;
			bpp = nbpp;
			levels = nlevels;
			align = nalign;
			pitch = align ? 0 : w*bpp;
			compressed = ncompressed;
			data = ndata ? ndata : new uchar[calcsize()];
			if(!ndata) { owner = this; freefunc = NULL; }
		}
  
		int calclevelsize(int level) const { return ((max(w>>level, 1)+align-1)/align)*((max(h>>level, 1)+align-1)/align)*bpp; }
 
		int calcsize() const
		{
			if(!align) return w*h*bpp;
			int lw = w, lh = h,
				size = 0;

			for(std::size_t i=0; i<levels; i++)
			{
				if(lw<=0) lw = 1;
				if(lh<=0) lh = 1;
				size += ((lw+align-1)/align)*((lh+align-1)/align)*bpp;
				if(lw*lh==1) break;
				lw >>= 1;
				lh >>= 1;
			}
			return size;
		}

		void disown()
		{
			data = NULL;
			owner = NULL;
			freefunc = NULL;
		}

		void cleanup()
		{
			if(owner==this) delete[] data;
			else if(freefunc) (*freefunc)(owner);
			disown();
		}

		void replace(ImageData &d)
		{
			cleanup();
			*this = d;
			if(owner == &d) owner = this;
			d.disown();
		}

		/*void wrap(SDL_Surface *s)
		{
			setdata((uchar *)s->pixels, s->w, s->h, s->format->BytesPerPixel);
			pitch = s->pitch;
			owner = s;
			freefunc = (void (*)(void *))SDL_FreeSurface;
		}*/

	};



	struct Slot
	{
		struct Tex
		{
			int type;
			Texture *t;
			string name;
			int combined;
		};

		int index;
		std::vector<Tex> sts;
		//Shader *shader;
		vector<ShaderParam> params;
		VSlot *variants;
		bool loaded;
		uint texmask;
		char *autograss;
		Texture *grasstex, *thumbnail;
		char *layermaskname;
		int layermaskmode;
		float layermaskscale;
		ImageData *layermask;
		bool ffenv;

		Slot(int index = -1) : index(index), variants(NULL), autograss(NULL), layermaskname(NULL), layermask(NULL) { reset(); }
    
		void reset()
		{
			sts.clear();
			//shader = NULL;
			params.clear();
			loaded = false;
			texmask = 0;
			DELETEA(autograss);
			grasstex = NULL;
			thumbnail = NULL;
			DELETEA(layermaskname);
			layermaskmode = 0;
			layermaskscale = 1;
			if(layermask) DELETEP(layermask);
			ffenv = false;
		}

		void cleanup()
		{
			loaded = false;
			grasstex = NULL;
			thumbnail = NULL;
			
			for(std::size_t i=0; i<sts.size(); i++)
			{
				Tex &t = sts[i];
				t.t = NULL;
				t.combined = -1;
			}
		}
	};

	
	// TODO: Move this into VSlot class?
	inline void VSlot::addvariant(Slot *slot)
	{
		if(!slot->variants) slot->variants = this;
		else
		{
			VSlot *prev = slot->variants;
			while(prev->next) prev = prev->next;
			prev->next = this;
		}
	}

};
};