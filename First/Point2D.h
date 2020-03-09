#pragma once
class Point2D
{
public:
	Point2D();
	~Point2D();
	int getRow();
	int getCol();
	void setRow(int r);
	void setCol(int c);
	void setHit(int h);
	int getHit();
	bool operator == (const Point2D &other)
	{
		return row == other.row && col == other.col;
	}
	int getpreColor();
	void setpreColor(int color);

private:
	int row, col;
	int preColor;
	int hit;
};

