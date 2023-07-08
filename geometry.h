#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>


//模板类，2d向量，用法是Vec2<int>(2d整形向量)、Vec2<float>(2d浮点数向量)
template <class t> 
struct Vec2 {
	union{
		struct {t u, v;};
		struct {t x, y;};
		t raw[2];
	};
	Vec2<t>() :x(t()), y(t()) {}
	Vec2<t>(t _x, t _y) : x(_x),y(_y) {}
	Vec2<t>(const Vec2<t>& v) { *this = v;  }
	Vec2<t>& operator=(const Vec2<t>& v)
	{
		if (this != &v) {
			this->x = v.x;
			this->y = v.y;
		}
		return *this;
	}
	inline Vec2<t> operator +(const Vec2<t> &V) const { return Vec2<t>(x+V.x, y+V.y); }//向量加法
	inline Vec2<t> operator -(const Vec2<t> &V) const { return Vec2<t>(x-V.x, y-V.y); }//向量减法
	inline Vec2<t> operator *(float f)          const { return Vec2<t>(x*f, y*f); }//向量的数乘运算
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);//向量输出
	inline t& operator[](const int idx){ if (idx <= 0) return x; else return y; }
};

//模板类，3d向量，用法是Vec3<int>(3d整形向量)、Vec3<float>(3d浮点数向量)
template <class t> 
struct Vec3 {
	union
	{
		struct {t x, y, z;};
		t raw[3];
	};
	Vec3<t>() :x(t()), y(t()), z(t()) {}
	Vec3<t>(t _x, t _y, t _z) : x(_x), y(_y), z(_z) {}
	Vec3<t>(const Vec3<t>& v) { *this = v; }
	Vec3<t>& operator=(const Vec3<t>& v)
	{
		if (this != &v) {
			this->x = v.x;
			this->y = v.y;
			this->z = v.z;
		}
		return *this;
	}
	inline Vec3<t> operator +(const Vec3<t>& V) const { return Vec3<t>(x + V.x, y + V.y, z + V.z); }//向量加法
	inline Vec3<t> operator -(const Vec3<t>& V) const { return Vec3<t>(x - V.x, y - V.y, z - V.z); }//向量减法
	inline Vec3<t> operator *(float f)          const { return Vec3<t>(x * f, y * f, z * f); }//向量的数乘运算
	inline Vec3<t> operator ^(const Vec3<t>& v) const { return Vec3<t>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }//向量叉乘运算（外积）
	inline t       operator *(const Vec3<t>& v) const { return x * v.x + y * v.y + z * v.z; }//向量的点乘（内积）

	float norm() const { return std::sqrt(x * x + y * y + z * z); }//向量的模长
	Vec3<t>& normalize(t l = 1) { *this = (*this) * (l / norm()); return *this; }//向量归一化

	template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);//向量输出
	inline t& operator[](const int idx) { if (idx <= 0) return x; else if (idx == 1) return y; else return z; }
};

typedef Vec2<float> Vec2f;//几个常用的模板特化，并且重命名
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;

template <class t> 
std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class t> 
std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}

const int DEFAULT_ALLOC = 4;

class Matrix
{
private:
	std::vector<std::vector<float>> m;
	int rows, cols;
public:
	Matrix(int r = DEFAULT_ALLOC, int c = DEFAULT_ALLOC);
	inline int nrows();
	inline int ncols();

	static Matrix identity(int dimensions);
	std::vector<float>& operator[](const int i);
	Matrix operator*(const Matrix& a);
	Matrix transpose();
	Matrix inverse();

	friend std::ostream& operator<<(std::ostream& s, Matrix& m);
};

#endif //__GEOMETRY_H__