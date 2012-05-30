
#include <cstdlib>

#include "ElasticBox.h"
#include "debug.h"

using namespace Debug;

#define BOX_CHILDREN 4

//enum SearchResults { OK, MORE, WIDTH, HEIGHT, };



ElasticBox::ElasticBox( int minSize, int maxSize )
{
	this->minSize = minSize;
	this->maxSize = maxSize;
	X = Y = 0;
	Width = Height = minSize;
	children = NULL;
}


ElasticBox::ElasticBox( int minSize )
{
	X = Y = 0;
	Width = Height = minSize;
	children = NULL;
}


ElasticBox::~ElasticBox( )
{
	if(children != NULL){
		for( int i=0; i < 4; ++i )
			if( children[i] != NULL )
				delete children[i];
		free(children);
	}
	children = NULL;
}


bool ElasticBox::InsertItem( int* x, int* y, int itemWidth, int itemHeight)
{
	SearchResults spaceFound = MORE;
	while ( ( spaceFound = FindSpace( x, y, itemWidth, itemHeight ) ) != OK ) {
		if ( Width * 2 > maxSize && Height * 2 > maxSize ){
			debug( GRAPHICS, "Textures too big to be placed in atlas.\n");
			return false;
		}
		switch (spaceFound)	{
			case WIDTH:
				if (Width * 2 > maxSize)
					Resize(0, 0, this->Width, this->Height *= 2);
				else
					Resize(0, 0, this->Width *= 2, this->Height);
				break;
			case HEIGHT:
				if (Height * 2 > maxSize)
					Resize(0, 0, this->Width *= 2, this->Height);
				else
					Resize(0, 0, this->Width, this->Height *= 2);
				break;
			default:
				Resize(0, 0, this->Width *= 2, this->Height *= 2);
				break;
		}
	}
	return true;
}


ElasticBox::SearchResults ElasticBox::FindSpace( int* x, int* y, int itemWidth, int itemHeight)
{
	SearchResults spaceFound = OK;

	if (itemWidth > Width && itemHeight > Height)
		return MORE;
	if (itemWidth > Width)
		return WIDTH;
	if (itemHeight > Height)
		return HEIGHT;

	if( children == NULL ){
		*x = this->X;
		*y = this->Y;
		this->itemWidth = itemWidth;
		this->itemHeight = itemHeight;
		spaceFound = OK;
		children = (ElasticBox**)malloc(sizeof(ElasticBox*) * BOX_CHILDREN);
		for ( int i = 0; i < BOX_CHILDREN; i++ )
			children[i] = new ElasticBox(minSize);
		AdjustChildren();
	}else{
		int i;
		for ( i = 0; i < BOX_CHILDREN; i++ ){
			if ( children[i] != NULL &&
				( spaceFound = children[i]->FindSpace( x, y, itemWidth, itemHeight ) ) == OK )
				break;
		}
		if ( spaceFound == OK ){
			if ( i < 2 ){
				delete children[2];
				delete children[3];
				children[2] = children[3] = NULL;
			}else{
				delete children[0];
				delete children[1];
				children[0] = children[1] = NULL;
			}
		}
	}

	return spaceFound;
}


void ElasticBox::AdjustChildren()
{
	if (children[0] != NULL)
		children[0]->Resize(X + itemWidth, Y, Width - itemWidth, Height - Y);
	if (children[1] != NULL)
		children[1]->Resize(X, Y + itemHeight, itemWidth, Height - itemHeight);
	if (children[2] != NULL)
		children[2]->Resize(X, Y + itemHeight, Width - X, Height - itemHeight);
	if (children[3] != NULL)
		children[3]->Resize(X + itemWidth, Y, Width - itemWidth, itemHeight);
}


void ElasticBox::Resize( int x, int y, int width, int height )
{
	this->X = x;
	this->Y = y;
	this->Width = width;
	this->Height = height;
	if (children != NULL)
		AdjustChildren();
}
