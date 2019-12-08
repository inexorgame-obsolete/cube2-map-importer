#include "Cube2MapOldVersionSupport.hpp"

#include <algorithm>


namespace inexor {
namespace cube2_map_importer {


	ushort Cube2MapOldVersionSupport::encode_normal(const vec &n)
	{               
		#define PI  (3.1415927f)
		#define RAD (PI / 180.0f)

		if(n.iszero()) return 0;
		int yaw = int(-atan2(n.x, n.y)/RAD), pitch = int(asin(n.z)/RAD);
		return ushort(clamp(pitch + 90, 0, 180)*360 + (yaw < 0 ? yaw%360 + 360 : yaw%360) + 1);
	}
	

	std::shared_ptr<OctreeCubeAdditionalData> Cube2MapOldVersionSupport::new_cube_extension(OctreeCube &c, int maxverts, bool init)
	{
		if(c.extension && c.extension->maxverts >= maxverts)
		{
			return c.extension;
		}

		// Use copy constructor to create a new instance.
		std::shared_ptr<OctreeCubeAdditionalData> new_extension = std::make_shared<OctreeCubeAdditionalData>(c.extension, maxverts);
		
		if(init)
		{
			if(c.extension)
			{
				// Copy surface information.
				memcpy(new_extension->surfaces, c.extension->surfaces, sizeof(c.extension->surfaces));
				
				// Copy vertex information.
				// TODO: Does this work correctly?
				new_extension->vertices = c.extension->vertices;
			}
			else
			{
				// Reset surface memory.
				new_extension->reset_surfaces();
			}
		}
		
		// Copy cube extension.
		c.extension = new_extension;

		return new_extension;
	}


	void Cube2MapOldVersionSupport::edgespan2vectorcube(std::shared_ptr<OctreeCube> &c)
	{
		// TODO: Resolve macros!
		#define isempty(c) ((c).faces[0]==F_EMPTY)
		#define isentirelysolid(c) ((c).faces[0]==F_SOLID && (c).faces[1]==F_SOLID && (c).faces[2]==F_SOLID)
		#define loop(v,m) for(int v = 0; v<int(m); v++)
	
		#define edgemake(a, b) ((b)<<4|a)
		#define edgeset(edge, coord, val) ((edge) = ((coord) ? ((edge)&0xF)|((val)<<4) : ((edge)&0xF0)|(val)))

		if(isentirelysolid(*c) || isempty(*c)) return;

		OctreeCube o = *c;

		loop(x, 2) loop(y, 2) loop(z, 2)
		{
			ivec p(8*x, 8*y, 8*z);
			vec v;
			genedgespanvert(p, o, v);

			edgeset(cubeedge(*c, 0, y, z), x, int(v.x+0.49f));
			edgeset(cubeedge(*c, 1, z, x), y, int(v.y+0.49f));
			edgeset(cubeedge(*c, 2, x, y), z, int(v.z+0.49f));
		}
	}


	void Cube2MapOldVersionSupport::genedgespanvert(ivec &p, OctreeCube &c, vec &v)
	{
		ivec p1(8-p.x, p.y, p.z);
		ivec p2(p.x, 8-p.y, p.z);
		ivec p3(p.x, p.y, 8-p.z);

		plane plane1, plane2, plane3;
		genvertp(c, p, p1, p2, plane1);
		genvertp(c, p, p2, p3, plane2);
		genvertp(c, p, p3, p1, plane3);

		if(plane1==plane2) genvertp(c, p, p1, p2, plane1, true);
		if(plane1==plane3) genvertp(c, p, p1, p2, plane1, true);
		if(plane2==plane3) genvertp(c, p, p2, p3, plane2, true);

		using namespace std;

		v.x = max(0.0f, min(8.0f, v.x));
		v.y = max(0.0f, min(8.0f, v.y));
		v.z = max(0.0f, min(8.0f, v.z));

	}


	void Cube2MapOldVersionSupport::set_surfaces(OctreeCube &c, const SurfaceInfo *surfs, std::vector<VertexInfo>& verts, int numverts)
	{
		if(!c.extension || c.extension->maxverts < numverts)
		{
			new_cube_extension(c, numverts, false);
		}

		// Copy surface information
		memcpy(c.extension->surfaces, surfs, sizeof(c.extension->surfaces));

		// Copy vertex information
		c.extension->vertices = verts;
	}
	

	int Cube2MapOldVersionSupport::convert_old_material(int mat)
	{
		return ((mat&7)<<MATF_VOLUME_SHIFT) | (((mat>>3)&3)<<MATF_CLIP_SHIFT) | (((mat>>5)&7)<<MATF_FLAG_SHIFT);
	}


	void Cube2MapOldVersionSupport::fix_entity_data(Entity &e, const int version)
	{
		if(version <= 10 && e.type >= 7)
		{
			e.type++;
		}
		if(version <= 12 && e.type >= 8)
		{
			e.type++;
		}
		if(version <= 14 && e.type >= ET_MAPMODEL && e.type <= 16)
		{
			if(e.type == 16)
			{
				e.type = ET_MAPMODEL;
			}
			else
			{
				e.type++;
			}
		}
		if(version <= 20 && e.type >= ET_ENVMAP)
		{
			e.type++;
		}
		if(version <= 21 && e.type >= ET_PARTICLES)
		{
			e.type++;
		}
		if(version <= 22 && e.type >= ET_SOUND)
		{
			e.type++;
		}
		if(version <= 23 && e.type >= ET_SPOTLIGHT)
		{
			e.type++;
		}
		if(version <= 30 && (e.type == ET_MAPMODEL || e.type == ET_PLAYERSTART))
		{
			e.attr1 = (int(e.attr1)+180)%360;
		}
		if(version <= 31 && e.type == ET_MAPMODEL)
		{
			int yaw = (int(e.attr1)%360 + 360)%360 + 7;
			e.attr1 = yaw - yaw%15;
		}
	}


	void Cube2MapOldVersionSupport::genvertp(OctreeCube &c, ivec &p1, ivec &p2, ivec &p3, plane &pl, bool solid)
	{
		int dim = 0;
		if(p1.y==p2.y && p2.y==p3.y) dim = 1;
		else if(p1.z==p2.z && p2.z==p3.z) dim = 2;

		int coord = p1[dim];
		ivec v1(p1), v2(p2), v3(p3);
		v1[dim] = solid ? coord*8 : edgeval(c, p1, dim, coord);
		v2[dim] = solid ? coord*8 : edgeval(c, p2, dim, coord);
		v3[dim] = solid ? coord*8 : edgeval(c, p3, dim, coord);

		pl.toplane(v1.tovec(), v2.tovec(), v3.tovec());
	}


	int Cube2MapOldVersionSupport::edgeval(OctreeCube &c, const ivec &p, int dim, int coord)
	{
		return edgeget(cubeedge(c, dim, p[R[dim]]>>3, p[C[dim]]>>3), coord);

	}


	void Cube2MapOldVersionSupport::genfaceverts(const OctreeCube &c, int orient, ivec v[4])
	{

		switch(orient)
		{
	#define GENFACEORIENT(o, v0, v1, v2, v3) \
			case o: v0 v1 v2 v3 break;
	#define GENFACEVERT(o, n, x,y,z, xv,yv,zv) \
				v[n] = ivec(edgeget(cubeedge(c, 0, y, z), x), \
							edgeget(cubeedge(c, 1, z, x), y), \
							edgeget(cubeedge(c, 2, x, y), z));
			GENFACEVERTS(0, 1, 0, 1, 0, 1, , , , , , )
		#undef GENFACEORIENT
		#undef GENFACEVERT
		}
	}


	void Cube2MapOldVersionSupport::convert_old_surfaces(std::shared_ptr<OctreeCube> &c, const ivec &co, int size, SurfaceCompat *srcsurfs, int hassurfs, NormalsCompat *normals, int hasnorms, MergeCompat *merges, int hasmerges)
	{
		SurfaceInfo dstsurfs[6];

		std::vector<VertexInfo> verts(6*2*MAXFACEVERTS);
		
		int totalverts = 0;
		int numsurfs = 6;
		
		// TODO: Let the constructor do this!
		memset(dstsurfs, 0, sizeof(dstsurfs));
		
		for(std::size_t i=0; i<6; i++)
		{
			if((hassurfs|hasnorms|hasmerges)&(1<<i))
			{
				SurfaceInfo &dst = dstsurfs[i];
				
				VertexInfo *curverts = NULL;
				
				SurfaceCompat *src = NULL;
				
				SurfaceCompat *blend = NULL;
				
				int numverts = 0;

				if(hassurfs&(1<<i))
				{
					src = &srcsurfs[i];
					if(src->layer&2) 
					{ 
						blend = &srcsurfs[numsurfs++];

						dst.lmid[0] = src->lmid;
						dst.lmid[1] = blend->lmid;
						dst.numverts |= LAYER_BLEND;
						
						if(blend->lmid >= LMID_RESERVED && (src->x != blend->x || src->y != blend->y || src->w != blend->w || src->h != blend->h || memcmp(src->texcoords, blend->texcoords, sizeof(src->texcoords))))
						{
							dst.numverts |= LAYER_DUP;
						}
					}
					else if(src->layer == 1)
					{
						dst.lmid[1] = src->lmid;
						dst.numverts |= LAYER_BOTTOM;
					}
					else
					{
						dst.lmid[0] = src->lmid;
						dst.numverts |= LAYER_TOP;
					}
				}
				else
				{
					dst.numverts |= LAYER_TOP;
				}

				bool uselms = hassurfs&(1<<i) && (dst.lmid[0] >= LMID_RESERVED || dst.lmid[1] >= LMID_RESERVED || dst.numverts&~LAYER_TOP);
				bool usemerges = hasmerges&(1<<i) && merges[i].u1 < merges[i].u2 && merges[i].v1 < merges[i].v2;
				bool usenorms = hasnorms&(1<<i) && normals[i].normals[0] != bvec(128, 128, 128);

				if(uselms || usemerges || usenorms)
				{
					ivec v[4], pos[4], e1, e2, e3, n, vo = ivec(co).mask(0xFFF).shl(3);
					
					genfaceverts(*c, i, v); 

					n.cross((e1 = v[1]).sub(v[0]), (e2 = v[2]).sub(v[0]));
					
					if(usemerges)
					{
						const MergeCompat &m = merges[i];
					
						int offset = -n.dot(v[0].mul(size).add(vo));
						int dim = ((i)>>1);
						int vc = C[dim];
						int vr = R[dim];
					
						for(std::size_t k=0; k<4; k++)
						{
							const ivec &coords = facecoords[i][k];

							int cc = coords[vc] ? m.u2 : m.u1,
								rc = coords[vr] ? m.v2 : m.v1,
								dc = -(offset + n[vc]*cc + n[vr]*rc)/n[dim];

							ivec &mv = pos[k];

							mv[vc] = cc;
							mv[vr] = rc;
							mv[dim] = dc;
						}
					}
					else
					{
						int convex = (e3 = v[0]).sub(v[3]).dot(n);
						int vis = 3;

						if(!convex)
						{
							if(ivec().cross(e3, e2).iszero())
							{
								if(!n.iszero())
								{
									vis = 1;
								}
							} 
							else
							{
								if(n.iszero())
								{
									vis = 2;
								}
							}
						}
						int order = convex < 0 ? 1 : 0;

						pos[0] = v[order].mul(size).add(vo);
						pos[1] = vis&1 ? v[order+1].mul(size).add(vo) : pos[0];
						pos[2] = v[order+2].mul(size).add(vo);
						pos[3] = vis&2 ? v[(order+3)&3].mul(size).add(vo) : pos[0];
					}

					// TODO: Does this work?
					curverts = &verts[0] + totalverts;

					for(std::size_t k=0; k<4; k++)
					{
						if(k > 0 && (pos[k] == pos[0] || pos[k] == pos[k-1]))
						{
							continue;
						}

						VertexInfo &dv = curverts[numverts++];

						dv.setxyz(pos[k]);

						if(uselms)
						{
							float u = src->x + (src->texcoords[k*2] / 255.0f) * (src->w - 1),
								  v = src->y + (src->texcoords[k*2+1] / 255.0f) * (src->h - 1);

							dv.u = ushort(floor(clamp((u) * float(USHRT_MAX+1)/LM_PACKW + 0.5f, 0.0f, float(USHRT_MAX))));
							dv.v = ushort(floor(clamp((v) * float(USHRT_MAX+1)/LM_PACKH + 0.5f, 0.0f, float(USHRT_MAX))));
						}
						else
						{
							dv.u = dv.v = 0;
						}

						// TODO: Resolve this!
						dv.norm = usenorms && normals[i].normals[k] != bvec(128, 128, 128) ? encode_normal(normals[i].normals[k].tovec().normalize()) : 0;
					}
					
					dst.verts = totalverts;
					dst.numverts |= numverts;
					totalverts += numverts;

					if(dst.numverts&LAYER_DUP)
					{
						for(std::size_t k=0; k<4; k++)
						{
							if(k > 0 && (pos[k] == pos[0] || pos[k] == pos[k-1]))
							{
								continue;
							}
						
							VertexInfo &bv = verts[totalverts++];

							bv.setxyz(pos[k]);

							bv.u = ushort(floor(clamp((blend->x + (blend->texcoords[k*2] / 255.0f) * (blend->w - 1)) * float(USHRT_MAX+1)/LM_PACKW, 0.0f, float(USHRT_MAX))));
							bv.v = ushort(floor(clamp((blend->y + (blend->texcoords[k*2+1] / 255.0f) * (blend->h - 1)) * float(USHRT_MAX+1)/LM_PACKH, 0.0f, float(USHRT_MAX))));

							// TODO: Resolve!
							bv.norm = usenorms && normals[i].normals[k] != bvec(128, 128, 128) ? encode_normal(normals[i].normals[k].tovec().normalize()) : 0;
						}
					}
				}    
			}
		}

		set_surfaces(*c, dstsurfs, verts, totalverts);
	}

};
};
