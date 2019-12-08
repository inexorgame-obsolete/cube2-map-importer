#pragma once

// sqrtf function
#include <math.h>
#include <algorithm>


// Pre-declare structures.
struct vec2;
struct vec4;


// Clamp values in range.
template<class T, class U>
static inline T clamp(T a, U b, U c)
{
    return std::max(T(b), std::min(a, T(c)));
}


// A structure for vectors.
struct vec
{
    union
    {
        struct { float x, y, z; };
        struct { float r, g, b; };
        float v[3];
    };

    vec() {}
    explicit vec(int a) : x(static_cast<float>(a)), y(static_cast<float>(a)), z(static_cast<float>(a)) {} 
    explicit vec(float a) : x(a), y(a), z(a) {} 
    vec(float a, float b, float c) : x(a), y(b), z(c) {}
    explicit vec(int v[3]) : x(static_cast<float>(v[0])), y(static_cast<float>(v[1])), z(static_cast<float>(v[2])) {}
    explicit vec(float *v) : x(v[0]), y(v[1]), z(v[2]) {}
    explicit vec(const vec4 &v);
    explicit vec(const vec2 &v, float z = 0);

    vec(float yaw, float pitch) : x(-sinf(yaw)*cosf(pitch)), y(cosf(yaw)*cosf(pitch)), z(sinf(pitch)) {}

    float &operator[](int i)       { return v[i]; }
    float  operator[](int i) const { return v[i]; }
    
    vec &set(int i, float f) { v[i] = f; return *this; }

    bool operator==(const vec &o) const { return x == o.x && y == o.y && z == o.z; }
    bool operator!=(const vec &o) const { return x != o.x || y != o.y || z != o.z; }

    bool iszero() const { return x==0 && y==0 && z==0; }
    float squaredlen() const { return x*x + y*y + z*z; }
    float dot2(const vec &o) const { return x*o.x + y*o.y; }
    float dot(const vec &o) const { return x*o.x + y*o.y + z*o.z; }
    vec &mul(const vec &o)   { x *= o.x; y *= o.y; z *= o.z; return *this; }
    vec &mul(float f)        { x *= f; y *= f; z *= f; return *this; }
    vec &div(const vec &o)   { x /= o.x; y /= o.y; z /= o.z; return *this; }
    vec &div(float f)        { x /= f; y /= f; z /= f; return *this; }
    vec &add(const vec &o)   { x += o.x; y += o.y; z += o.z; return *this; }
    vec &add(float f)        { x += f; y += f; z += f; return *this; }
    vec &sub(const vec &o)   { x -= o.x; y -= o.y; z -= o.z; return *this; }
    vec &sub(float f)        { x -= f; y -= f; z -= f; return *this; }
    vec &neg2()              { x = -x; y = -y; return *this; }
    vec &neg()               { x = -x; y = -y; z = -z; return *this; }
    vec &min(const vec &o)   { x = std::min(x, o.x); y = std::min(y, o.y); z = std::min(z, o.z); return *this; }
    vec &max(const vec &o)   { x = std::max(x, o.x); y = std::max(y, o.y); z = std::max(z, o.z); return *this; }
    vec &min(float f)        { x = std::min(x, f); y = std::min(y, f); z = std::min(z, f); return *this; }
    vec &max(float f)        { x = std::max(x, f); y = std::max(y, f); z = std::max(z, f); return *this; }
    vec &clamp(float f, float h) { x = ::clamp(x, f, h); y = ::clamp(y, f, h); z = ::clamp(z, f, h); return *this; }
    float magnitude2() const { return sqrtf(dot2(*this)); }
    float magnitude() const  { return sqrtf(squaredlen()); }
    vec &normalize()         { div(magnitude()); return *this; }
    bool isnormalized() const { float m = squaredlen(); return (m>0.99f && m<1.01f); }
    float squaredist(const vec &e) const { return vec(*this).sub(e).squaredlen(); }
    float dist(const vec &e) const { vec t; return dist(e, t); }
    float dist(const vec &e, vec &t) const { t = *this; t.sub(e); return t.magnitude(); }
    float dist2(const vec &o) const { float dx = x-o.x, dy = y-o.y; return sqrtf(dx*dx + dy*dy); }
    bool reject(const vec &o, float r) { return x>o.x+r || x<o.x-r || y>o.y+r || y<o.y-r; }
    template<class A, class B>
    vec &cross(const A &a, const B &b) { x = a.y*b.z-a.z*b.y; y = a.z*b.x-a.x*b.z; z = a.x*b.y-a.y*b.x; return *this; }
    vec &cross(const vec &o, const vec &a, const vec &b) { return cross(vec(a).sub(o), vec(b).sub(o)); }
    float scalartriple(const vec &a, const vec &b) const { return x*(a.y*b.z-a.z*b.y) + y*(a.z*b.x-a.x*b.z) + z*(a.x*b.y-a.y*b.x); }
    vec &reflectz(float rz) { z = 2*rz - z; return *this; }
    vec &reflect(const vec &n) { float k = 2*dot(n); x -= k*n.x; y -= k*n.y; z -= k*n.z; return *this; }
    vec &project(const vec &n) { float k = dot(n); x -= k*n.x; y -= k*n.y; z -= k*n.z; return *this; }
    vec &projectxydir(const vec &n) { if(n.z) z = -(x*n.x/n.z + y*n.y/n.z); return *this; }
    vec &projectxy(const vec &n)
    {
        float m = squaredlen(), k = dot(n);
        projectxydir(n);
        rescale(sqrtf(std::max(m - k*k, 0.0f)));
        return *this;
    }
    vec &projectxy(const vec &n, float threshold)
    {
        float m = squaredlen(), k = std::min(dot(n), threshold);
        projectxydir(n);
        rescale(sqrtf(std::max(m - k*k, 0.0f)));
        return *this;
    }
    vec &lerp(const vec &b, float t) { x += (b.x-x)*t; y += (b.y-y)*t; z += (b.z-z)*t; return *this; }
    vec &lerp(const vec &a, const vec &b, float t) { x = a.x + (b.x-a.x)*t; y = a.y + (b.y-a.y)*t; z = a.z + (b.z-a.z)*t; return *this; }

    vec &rescale(float k)
    {
        float mag = magnitude();
        if(mag > 1e-6f) mul(k / mag);
        return *this;
    }

    vec &rotate_around_z(float c, float s) { float rx = x, ry = y; x = c*rx-s*ry; y = c*ry+s*rx; return *this; }
    vec &rotate_around_x(float c, float s) { float ry = y, rz = z; y = c*ry-s*rz; z = c*rz+s*ry; return *this; }
    vec &rotate_around_y(float c, float s) { float rx = x, rz = z; x = c*rx-s*rz; z = c*rz+s*rx; return *this; }

    vec &rotate_around_z(float angle) { return rotate_around_z(cosf(angle), sinf(angle)); }
    vec &rotate_around_x(float angle) { return rotate_around_x(cosf(angle), sinf(angle)); }
    vec &rotate_around_y(float angle) { return rotate_around_y(cosf(angle), sinf(angle)); }

    vec &rotate(float angle, const vec &d)
    {
        float c = cosf(angle), s = sinf(angle);
        return rotate(c, s, d);
    }

    vec &rotate(float c, float s, const vec &d)
    {
        *this = vec(x*(d.x*d.x*(1-c)+c) + y*(d.x*d.y*(1-c)-d.z*s) + z*(d.x*d.z*(1-c)+d.y*s),
                    x*(d.y*d.x*(1-c)+d.z*s) + y*(d.y*d.y*(1-c)+c) + z*(d.y*d.z*(1-c)-d.x*s),
                    x*(d.x*d.z*(1-c)-d.y*s) + y*(d.y*d.z*(1-c)+d.x*s) + z*(d.z*d.z*(1-c)+c));
        return *this;
    }

    void orthogonal(const vec &d)
    {
        int i = fabs(d.x) > fabs(d.y) ? (fabs(d.x) > fabs(d.z) ? 0 : 2) : (fabs(d.y) > fabs(d.z) ? 1 : 2); 
        v[i] = d[(i+1)%3];
        v[(i+1)%3] = -d[i];
        v[(i+2)%3] = 0;
    }

    void orthonormalize(vec &s, vec &t) const
    {
        s.sub(vec(*this).mul(dot(s)));
        t.sub(vec(*this).mul(dot(t)))
         .sub(vec(s).mul(s.dot(t)));
    }

    template<class T> float dist_to_bb(const T &min, const T &max) const
    {
        float sqrdist = 0;
        for(std::size_t i=0; i<3; i++)
		{
            if     (v[i] < min[i]) { float delta = v[i]-min[i]; sqrdist += delta*delta; }
            else if(v[i] > max[i]) { float delta = max[i]-v[i]; sqrdist += delta*delta; }
        }
        return sqrtf(sqrdist);
    }

    template<class T, class S> float dist_to_bb(const T &o, S size) const
    {
        return dist_to_bb(o, T(o).add(size));
    }

};



// A class for 4 dimensional vectors.
struct vec4
{
    union
    {
        struct { float x, y, z, w; };
        float v[4];
    };

    vec4() {}
    explicit vec4(const vec &p, float w = 0) : x(p.x), y(p.y), z(p.z), w(w) {}
    vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    float &operator[](int i)       { return v[i]; }
    float  operator[](int i) const { return v[i]; }

    float dot3(const vec4 &o) const { return x*o.x + y*o.y + z*o.z; }
    float dot3(const vec &o) const { return x*o.x + y*o.y + z*o.z; }
    float dot(const vec4 &o) const { return dot3(o) + w*o.w; }
    float dot(const vec &o) const  { return x*o.x + y*o.y + z*o.z + w; }
    float squaredlen() const { return dot(*this); }
    float magnitude() const  { return sqrtf(squaredlen()); }
    float magnitude3() const { return sqrtf(dot3(*this)); }
    vec4 &normalize() { mul(1/magnitude()); return *this; }

    vec4 &lerp(const vec4 &b, float t)
    {
        x += (b.x-x)*t;
        y += (b.y-y)*t;
        z += (b.z-z)*t;
        w += (b.w-w)*t;
        return *this;
    }
    vec4 &lerp(const vec4 &a, const vec4 &b, float t) 
    { 
        x = a.x+(b.x-a.x)*t; 
        y = a.y+(b.y-a.y)*t; 
        z = a.z+(b.z-a.z)*t;
        w = a.w+(b.w-a.w)*t;
        return *this;
    }

    vec4 &mul3(float f)      { x *= f; y *= f; z *= f; return *this; }
    vec4 &mul(float f)       { mul3(f); w *= f; return *this; }
    vec4 &mul(const vec4 &o) { x *= o.x; y *= o.y; z *= o.z; w *= o.w; return *this; }
    vec4 &div3(float f)      { x /= f; y /= f; z /= f; return *this; }
    vec4 &div(float f)       { div3(f); w /= f; return *this; }
    vec4 &div(const vec4 &o) { x /= o.x; y /= o.y; z /= o.z; w /= o.w; return *this; }
    vec4 &add(const vec4 &o) { x += o.x; y += o.y; z += o.z; w += o.w; return *this; }
    vec4 &addw(float f)      { w += f; return *this; }
    vec4 &sub(const vec4 &o) { x -= o.x; y -= o.y; z -= o.z; w -= o.w; return *this; }
    vec4 &subw(float f)      { w -= f; return *this; }
    vec4 &neg3()             { x = -x; y = -y; z = -z; return *this; }
    vec4 &neg()              { neg3(); w = -w; return *this; }

    void setxyz(const vec &v) { x = v.x; y = v.y; z = v.z; }
};



// A class for 2 dimensional vectors.
struct vec2
{
    union
    {
        struct { float x, y; };
        float v[2];
    };

    vec2() {}
    vec2(float x, float y) : x(x), y(y) {}
    explicit vec2(const vec &v) : x(v.x), y(v.y) {}
    explicit vec2(const vec4 &v) : x(v.x), y(v.y) {}

    float &operator[](int i)       { return v[i]; }
    float  operator[](int i) const { return v[i]; }

    bool operator==(const vec2 &o) const { return x == o.x && y == o.y; }
    bool operator!=(const vec2 &o) const { return x != o.x || y != o.y; }

    bool iszero() const { return x==0 && y==0; }
    float dot(const vec2 &o) const  { return x*o.x + y*o.y; }
    float squaredlen() const { return dot(*this); }
    float magnitude() const  { return sqrtf(squaredlen()); }
    vec2 &normalize() { mul(1/magnitude()); return *this; }
    float cross(const vec2 &o) const { return x*o.y - y*o.x; }

    vec2 &mul(float f)       { x *= f; y *= f; return *this; }
    vec2 &mul(const vec2 &o) { x *= o.x; y *= o.y; return *this; }
    vec2 &div(float f)       { x /= f; y /= f; return *this; }
    vec2 &div(const vec2 &o) { x /= o.x; y /= o.y; return *this; }
    vec2 &add(float f)       { x += f; y += f; return *this; }
    vec2 &add(const vec2 &o) { x += o.x; y += o.y; return *this; }
    vec2 &sub(float f)       { x -= f; y -= f; return *this; }
    vec2 &sub(const vec2 &o) { x -= o.x; y -= o.y; return *this; }
    vec2 &neg()              { x = -x; y = -y; return *this; }
};


// 
struct plane : vec
{
    float offset;

    float dist(const vec &p) const { return dot(p)+offset; }
    bool operator==(const plane &p) const { return x==p.x && y==p.y && z==p.z && offset==p.offset; }
    bool operator!=(const plane &p) const { return x!=p.x || y!=p.y || z!=p.z || offset!=p.offset; }

    plane() {}
    plane(const vec &c, float off) : vec(c), offset(off) {} 
    plane(const vec4 &p) : vec(p), offset(p.w) {}
    plane(int d, float off)
    {
        x = y = z = 0.0f;
        v[d] = 1.0f;
        offset = -off;
    }
    plane(float a, float b, float c, float d) : vec(a, b, c), offset(d) {}

    void toplane(const vec &n, const vec &p)
    {
        x = n.x; y = n.y; z = n.z;
        offset = -dot(p);
    }

    bool toplane(const vec &a, const vec &b, const vec &c)
    {
        cross(vec(b).sub(a), vec(c).sub(a));
        float mag = magnitude();
        if(!mag) return false;
        div(mag);
        offset = -dot(a);
        return true;
    }

    bool rayintersect(const vec &o, const vec &ray, float &dist)
    {
        float cosalpha = dot(ray);
        if(cosalpha==0) return false;
        float deltac = offset+dot(o);
        dist -= deltac/cosalpha;
        return true;
    }

    plane &reflectz(float rz)
    {
        offset += 2*rz*z;
        z = -z;
        return *this; 
    }

    plane &invert()
    {
        neg();
        offset = -offset;
        return *this;
    }

    plane &scale(float k)
    {
        mul(k);
        return *this;
    }

    plane &translate(const vec &p)
    {
        offset += dot(p);
        return *this;
    }

    plane &normalize()
    {
        float mag = magnitude();
        div(mag);
        offset /= mag; 
        return *this;
    }

    float zintersect(const vec &p) const { return -(x*p.x+y*p.y+offset)/z; }
    float zdelta(const vec &p) const { return -(x*p.x+y*p.y)/z; }
    float zdist(const vec &p) const { return p.z-zintersect(p); }
};



struct bvec
{
    union
    {
        struct { unsigned char x, y, z; };
        struct { unsigned char r, g, b; };
        unsigned char v[3];
    };

    bvec() {}
    bvec(unsigned char x, unsigned char y, unsigned char z) : x(x), y(y), z(z) {}
    bvec(const vec &v) : x((unsigned char)((v.x+1)*255/2)), y((unsigned char)((v.y+1)*255/2)), z((unsigned char)((v.z+1)*255/2)) {}

    unsigned char &operator[](int i)       { return v[i]; }
    unsigned char  operator[](int i) const { return v[i]; }

    bool operator==(const bvec &v) const { return x==v.x && y==v.y && z==v.z; }
    bool operator!=(const bvec &v) const { return x!=v.x || y!=v.y || z!=v.z; }

    bool iszero() const { return x==0 && y==0 && z==0; }

    vec tovec() const { return vec(x*(2.0f/255.0f)-1.0f, y*(2.0f/255.0f)-1.0f, z*(2.0f/255.0f)-1.0f); }

    bvec &normalize()
    {
        vec n(x-127.5f, y-127.5f, z-127.5f);
        float mag = 127.5f/n.magnitude();
        x = unsigned char(n.x*mag+127.5f);
        y = unsigned char(n.y*mag+127.5f);
        z = unsigned char(n.z*mag+127.5f);
        return *this;
    }

    void lerp(const bvec &a, const bvec &b, float t) { x = unsigned char(a.x + (b.x-a.x)*t); y = unsigned char(a.y + (b.y-a.y)*t); z = unsigned char(a.z + (b.z-a.z)*t); }

    void flip() { x -= 128; y -= 128; z -= 128; }

    bvec &shl(int n) { x<<= n; y<<= n; z<<= n; return *this; }
    bvec &shr(int n) { x>>= n; y>>= n; z>>= n; return *this; }

    static bvec fromcolor(const vec &v) { return bvec(unsigned char(v.x*255.0f), unsigned char(v.y*255.0f), unsigned char(v.z*255.0f)); }
    vec tocolor() const { return vec(x*(1.0f/255.0f), y*(1.0f/255.0f), z*(1.0f/255.0f)); }
};


// DIM: X=0 Y=1 Z=2.
const int R[3]  = {1, 2, 0}; // row
const int C[3]  = {2, 0, 1}; // col
const int D[3]  = {0, 1, 2}; // depth



// A class for integer vectors.
struct ivec
{
    union
    {
        struct { int x, y, z; };
        struct { int r, g, b; };
        int v[3];
    };

    ivec() {}
    ivec(const vec &v) : x(int(v.x)), y(int(v.y)), z(int(v.z)) {}
    explicit ivec(int i)
    {
        x = ((i&1)>>0);
        y = ((i&2)>>1);
        z = ((i&4)>>2);
    }
    ivec(int a, int b, int c) : x(a), y(b), z(c) {}
    ivec(int d, int row, int col, int depth)
    {
        v[R[d]] = row;
        v[C[d]] = col;
        v[D[d]] = depth;
    }
    ivec(int i, int cx, int cy, int cz, int size)
    {
        x = cx+((i&1)>>0)*size;
        y = cy+((i&2)>>1)*size;
        z = cz+((i&4)>>2)*size;
    }
    vec tovec() const { return vec(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)); }
    int toint() const { return (x>0?1:0) + (y>0?2:0) + (z>0?4:0); }

    int &operator[](int i)       { return v[i]; }
    int  operator[](int i) const { return v[i]; }

    //int idx(int i) { return v[i]; }
    bool operator==(const ivec &v) const { return x==v.x && y==v.y && z==v.z; }
    bool operator!=(const ivec &v) const { return x!=v.x || y!=v.y || z!=v.z; }
    bool iszero() const { return x==0 && y==0 && z==0; }
    ivec &shl(int n) { x<<= n; y<<= n; z<<= n; return *this; }
    ivec &shr(int n) { x>>= n; y>>= n; z>>= n; return *this; }
    ivec &mul(int n) { x *= n; y *= n; z *= n; return *this; }
    ivec &div(int n) { x /= n; y /= n; z /= n; return *this; }
    ivec &add(int n) { x += n; y += n; z += n; return *this; }
    ivec &sub(int n) { x -= n; y -= n; z -= n; return *this; }
    ivec &mul(const ivec &v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
    ivec &div(const ivec &v) { x /= v.x; y /= v.y; z /= v.z; return *this; }
    ivec &add(const ivec &v) { x += v.x; y += v.y; z += v.z; return *this; }
    ivec &sub(const ivec &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    ivec &mask(int n) { x &= n; y &= n; z &= n; return *this; }
    ivec &neg() { return mul(-1); }
    ivec &min(const ivec &o) { x = std::min(x, o.x); y = std::min(y, o.y); z = std::min(z, o.z); return *this; }
    ivec &max(const ivec &o) { x = std::max(x, o.x); y = std::max(y, o.y); z = std::max(z, o.z); return *this; }
    ivec &min(int n) { x = std::min(x, n); y = std::min(y, n); z = std::min(z, n); return *this; }
    ivec &max(int n) { x = std::max(x, n); y = std::max(y, n); z = std::max(z, n); return *this; }
    ivec &abs() { x = ::abs(x); y = ::abs(y); z = ::abs(z); return *this; }
    ivec &cross(const ivec &a, const ivec &b) { x = a.y*b.z-a.z*b.y; y = a.z*b.x-a.x*b.z; z = a.x*b.y-a.y*b.x; return *this; }
    int dot(const ivec &o) const { return x*o.x + y*o.y + z*o.z; }
    float dist(const plane &p) const { return x*p.x + y*p.y + z*p.z + p.offset; }
};



// 
struct triangle
{
    vec a, b, c;

    triangle(const vec &a, const vec &b, const vec &c) : a(a), b(b), c(c) {}
    triangle() {}

    triangle &add(const vec &o) { a.add(o); b.add(o); c.add(o); return *this; }
    triangle &sub(const vec &o) { a.sub(o); b.sub(o); c.sub(o); return *this; }

    bool operator==(const triangle &t) const { return a == t.a && b == t.b && c == t.c; }
};
