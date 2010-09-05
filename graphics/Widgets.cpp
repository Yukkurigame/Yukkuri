/*
 * Widgets.cpp
 *
 *  Created on: 30.06.2010
 */
#include "Widgets.h"
#include "Luaconfig.h"
#include "config.h"

LuaConfig* cfg = LuaConfig::Instance();

Widget::Widget()
{
	graph = Graphics::Instance();
	Type = NONE;
	Width = 0;
	Height = 0;
	PosX = 0;
	PosY = 0;
	PosZ = 0;
	visible = true;
	Parent = NULL;
	background = NULL;
}

Widget::~Widget()
{
	graph->FreeGLSprite( background );
	Parent = NULL;
	//FIXME: parent dies but children overcomes this.
	Children.clear();
}

bool Widget::create( string name, string text, int x, int y )
{
	//FIXME: text not used it is bad;
	Texture* tex;
	tex = graph->LoadGLTexture( name );
	background = graph->CreateGLSprite( PosX, PosY, getZ(), x, y, Width, Height, tex );
	if(!background)
		return false;
	return true;
}

bool Widget::load( string config )
{
	cfg->getValue( "name", config, "widget", Name );
	cfg->getValue( "x", config, "widget", PosX );
	cfg->getValue( "y", config, "widget", PosY );
	cfg->getValue( "width", config, "widget", Width );
	cfg->getValue( "height", config, "widget", Height );

	int z = 0;
	cfg->getValue("depth", config, "widget", z );
	setZ( z );

	if( Type != NONE ){
		string imgname;
		string text;
		int bgx, bgy;
		cfg->getValue( "bgimage", config, "widget", imgname );
		cfg->getValue( "text", config, "widget", text );
		cfg->getValue( "bgposx", config, "widget", bgx );
		cfg->getValue( "bgposy", config, "widget", bgy );

		if( !create( imgname, text, bgx, bgy )){
			return NULL;
		}
	}
	return true;
}

float Widget::getZ( )
{
	extern MainConfig conf;
	return PosZ + conf.widgetsPosZ;
}

void Widget::setParent( Widget* p )
{
	extern MainConfig conf;
	Parent = p;
	PosX += p->getX( );
	PosY = p->getY( ) + p->getHeight( ) - PosY;
	PosZ = PosZ + p->getZ( ) - conf.widgetsPosZ + 0.1;
	if( background )
		background->vertices->z = getZ();
}

void Widget::addChild( Widget* child )
{
	Children.push_back( child );
}

void Widget::toggleVisibility( )
{
	if( this->visible )
		visible = false;
	else
		visible = true;
	if( this->background && this->background->visible != visible )
		this->background->toggleVisibility( );
	for( int i = 0, end = Children.size(); i < end; ++i ){
		Children[i]->toggleVisibility();
	}
}

TextWidget::TextWidget( )
{
	FontName = "DejaVuSans";
	FontSize = 12;
	textx = 0;
	texty = 0;
	Text = "";
	StaticTextSprite = NULL;
	TextSprite = NULL;
}

TextWidget::~TextWidget( )
{
	graph->FreeTextSprite( TextSprite );
	graph->FreeTextSprite( StaticTextSprite );
}

bool TextWidget::create( string name, string text, int x, int y )
{
	if( name == "" ){
		background = NULL;
	}else{
		if( !Widget::create( name, text, x, y ) )
			background = NULL;
	}
	if( text != "" ){
		StaticTextSprite = graph->CreateGLSprite( x, y, getZ(), 20, 20, Width, Height, NULL );
		StaticTextSprite->clr->set( 0 );
	}
	Text = text;
	TextSprite = graph->CreateGLSprite( x, y, getZ(), 20, 20, Width, Height, NULL );
	TextSprite->clr->set( 0 );
	return true;
}

bool TextWidget::load( string config )
{
	if( !Widget::load( config ) )
		return NULL;

	string font;
	float textx, texty;
	textx = texty = 0;
	int fontsize = 12;
	vector<int> vcolor;

	cfg->getValue( "textx", config, "widget", textx );
	cfg->getValue( "texty", config, "widget", texty );
	cfg->getValue( "font", config, "widget", font );
	cfg->getValue( "fontsize", config, "widget", fontsize );
	cfg->getValue( "fontcolor", config, "widget", vcolor );

	setFont( font, fontsize );
	setTextPosition( textx, texty );
	if( vcolor.size( ) > 2 )
		setFontColor(vcolor[0], vcolor[1], vcolor[2]);
	return true;
}

void TextWidget::setParent( Widget* p )
{
	float x = p->getX( ) + PosX + textx;
	float y = p->getY( ) + p->getHeight( ) - PosY - texty;
	Widget::setParent( p );
	textPosition( x, y );
}

void TextWidget::setFontColor( int r, int g, int b )
{
	if( StaticTextSprite )
		StaticTextSprite->clr->set( r, g, b );
	TextSprite->clr->set( r, g, b );
}

void TextWidget::setText( string text )
{
	if( AddText == text )
		return;
	if( StaticTextSprite ){
		if( Text != "" && !StaticTextSprite->tex ){
			graph->ChangeTextSprite( StaticTextSprite, FontName, FontSize, Text, 1 );
			TextSprite->setPosition( TextSprite->posx + StaticTextSprite->width, TextSprite->posy );
		}
	}
	AddText = text;
	graph->ChangeTextSprite( TextSprite, FontName, FontSize, AddText );
}

void TextWidget::setTextPosition( float x, float y )
{
	textx = x;
	texty = y;
	textPosition( PosX + x, PosY + Height - y );
}

void TextWidget::toggleVisibility( )
{
	Widget::toggleVisibility( );
	if( StaticTextSprite && StaticTextSprite->visible != visible )
		StaticTextSprite->toggleVisibility( );
	if( TextSprite && TextSprite->visible != visible )
		TextSprite->toggleVisibility( );
}

void TextWidget::textPosition( float x, float y )
{
	float txdelta;
	txdelta = 0;
	if( StaticTextSprite ){
		StaticTextSprite->setPosition( x, y, getZ( ) + 0.1 );
		txdelta = StaticTextSprite->width;
	}
	TextSprite->setPosition( x + txdelta, y, getZ( ) + 0.1 );
}

BarWidget::BarWidget()
{
	bar = NULL;
	top = NULL;
	barwidth = 0;
	barmaxvalue = 1;
	barstartx = 0;
}

BarWidget::~BarWidget( )
{
	graph->FreeGLSprite( top );
	graph->FreeGLSprite( bar );
}

bool BarWidget::load( string config )
{
	if( !TextWidget::load( config ) )
		return NULL;
	string imgname;
	int position[9];
	vector<int> vcolor;
	//Order: topimgx, topimgy, barx, bary, barwidth, barheight
	//Ya, it's cruve, but it's simple
	cfg->getValue( "topimgx", config, "widget", position[0] );
	cfg->getValue( "topimgy", config, "widget", position[1] );
	cfg->getValue( "barx", config, "widget", position[2] );
	cfg->getValue( "bary", config, "widget", position[3] );
	cfg->getValue( "barwidth", config, "widget", position[4] );
	cfg->getValue( "barheight", config, "widget", position[5] );
	cfg->getValue( "source", config, "widget", imgname );
	cfg->getValue( "barcolor", config, "widget", vcolor );
	if( vcolor.size( ) > 2 ){
		position[6] = vcolor[0]; //r
		position[7] = vcolor[1]; //g
		position[8] = vcolor[2]; //b
	}else{
		position[6] = 0; position[7] = 0; position[8] = 0;
	}
	createBar( imgname, position );
	return true;
}

void BarWidget::createBar( string name, int* pos)
{
	Texture* tex;
	barstartx = PosX + pos[2];
	if( pos[4] > 0 )
		barwidth = pos[4];
	else
		barwidth = Width;
	tex = graph->LoadGLTexture( name );
	bar = graph->CreateGLSprite( barstartx, PosY + pos[3], getZ(), barwidth, pos[5] );
	if( tex )
		top = graph->CreateGLSprite( PosX, PosY, getZ() + 0.1, pos[0], pos[1], Width, Height, tex );
	if( bar ){
		bar->clr->set( pos[6], pos[7], pos[8] );
	}
	setTextPosition( getTextX() + 5, getTextY() - pos[5] );
	setBarValue(1);
	setBarSize(1);
}

void BarWidget::setBarValue( int value )
{
	if( value == barvalue )
		return;
	barvalue = value;
	{//Output text;
		char str[25];
		sprintf( str, "%d/%d", value, barmaxvalue );
		setText( str );
	}
	if( value < 0 )
		value = 0;
	if( value > barmaxvalue )
		value = barmaxvalue;
	float s = static_cast<float>(value) / static_cast<float>(barmaxvalue);
	float x = barwidth * s;
	bar->resize( x, -1 );
}

void BarWidget::setParent( Widget* p )
{
	float bardelta = barstartx - PosX;
	float x = p->getX( ) + PosX;
	float y = p->getY( ) + p->getHeight( ) - PosY;
	float barheight = bar->vertices->lt.y - bar->vertices->lb.y;
	TextWidget::setParent( p );
	bar->setPosition( x + bardelta, y, getZ() );
	if( top )
		top->setPosition( x, y - barheight, getZ() + 0.01 );
}

void BarWidget::toggleVisibility( )
{
	TextWidget::toggleVisibility( );
	if( bar && bar->visible != visible )
		bar->toggleVisibility( );
	if( top && top->visible != visible )
			top->toggleVisibility( );
}
