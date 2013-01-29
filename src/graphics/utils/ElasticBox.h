/*
 * ElasticBox.h
 *
 *  Created on: 19.03.2012
 */

#ifndef ELASTICBOX_H_
#define ELASTICBOX_H_

class ElasticBox
{
public:
	int Width;
	int Height;

	ElasticBox( int minSize, int maxSize );
	ElasticBox( int minSize );
	~ElasticBox( );

	/// <summary>
	/// Добавляет новый элемент в контейнер.
	/// </summary>
	/// <param name="x">Возвращает X координату расположения нового элемента в контейнере.</param>
	/// <param name="y">Возвращает Y координату расположения нового элемента в контейнере.</param>
	/// <param name="itemWidth">Ширина нового элемента.</param>
	/// <param name="itemHeight">Высота нового элемента.</param>
	bool InsertItem( int* x, int* y, int itemWidth, int itemHeight);

private:
	enum SearchResults { OK, MORE, WIDTH, HEIGHT, };

	int X, Y;
	int itemWidth, itemHeight;
	int minSize, maxSize;

	ElasticBox** children;

	SearchResults FindSpace( int* x, int* y, int itemWidth, int itemHeight);

	void AdjustChildren();
	void Resize(int x, int y, int width, int height);
};

// Pack similar rectangles int pod container
struct ElasticRectPODBox
{
	int maxSize;
	int Width;
	int Height;
	int cols;
	int rows;

	ElasticRectPODBox( int mSize );
	bool calculate( int width, int height, int count );
};

#endif /* ELASTICBOX_H_ */
