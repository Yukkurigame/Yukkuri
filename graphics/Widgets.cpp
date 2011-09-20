/*
 * Widgets.cpp
 *
 *  Created on: 30.06.2010
 */
#include "Widgets.h"
#include "Luaconfig.h"
#include "config.h"

Widget::Widget()
{
	graph = Graphics::Instance();
	Type = NONE;
	Width = 0;
	Height = 0;
	PosX = 0;
	PosY = 0;
	PosZ = 0;
	Align = NONE;
	visible = true;
	Parent = NULL;
	background = NULL;
	Binded = NULL;
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
	LuaConfig* cfg = new LuaConfig;
	cfg->getValue( "name", config, "widget", Name );
	cfg->getValue( "x", config, "widget", OffsetX );
	cfg->getValue( "y", config, "widget", OffsetY );
	cfg->getValue( "width", config, "widget", Width );
	cfg->getValue( "height", config, "widget", Height );

	cfg->getValue( "align", config, "widget", Align );
	cfg->getValue( "valign", config, "widget", VAlign );
	updatePosition( );

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
			return false;
		}
	}

	delete cfg;

	return true;
}

void Widget::resize( float w, float h )
{
	if( w == Width && h == Height )
		return;
	if( w >= 0 )
		Width = w;
	if( h >= 0 )
		Height = h;
	if( background )
		background->resize( Width, Height );
}

void Widget::updatePosition( )
{
	extern MainConfig conf;
	float posx, posy, startx, starty, width, height;
	if(Parent){
		width = Parent->getWidth( );
		height = Parent->getHeight( );
		startx = Parent->getX( );
		starty = Parent->getY( ) + height;
	}else{
		startx = 0;
		starty = conf.windowHeight;
		width = conf.windowWidth;
		height = conf.windowHeight;
	}
	switch(Align){
		case CENTER:
			posx = startx + width * 0.5f - this->Width * 0.5f + OffsetX;
			break;
		case RIGHT:
			posx = startx + width - this->Width + OffsetX;
			break;
		case LEFT:
		default:
			posx = startx + OffsetX;
			break;
	}
	switch(VAlign){
		case CENTER:
			posy = starty - height * 0.5f + this->Height * 0.5f - OffsetY;
			break;
		case BOTTOM:
			posy = starty - height + this->Height - OffsetY;
			break;
		case TOP:
		default:
			posy = starty - OffsetY - this->Height;
			break;
	}
	PosX = posx;
	PosY = posy;
	if( background ){
		background->setPosition( PosX, PosY, getZ() );
	}

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
	PosZ = PosZ + p->getZ( ) - conf.widgetsPosZ + 0.1f;
	updatePosition();
}

void Widget::addChild( Widget* child )
{
	Children.push_back( child );
}

/* This function needs memory allocation.
 */
void Widget::getChildren( Widget* children[], int size )
{
	for( int i = 0, vsz = Children.size(); i < vsz; ++i ){
		if( i >= size ) break;
		children[i] = Children[i];
	}
}

bool Widget::bindValue( float* val )
{
	if( val ){
		Binded = val;
		return true;
	}
	return false;
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
	TextX = 0;
	TextY = 0;
	TextAlign = NONE;
	Text = "";
	StaticTextSprite = NULL;
	TextSprite = NULL;
	BindedCache = 0.00123f;
}

TextWidget::~TextWidget( )
{
	graph->FreeTextSprite( &TextSprite );
	graph->FreeTextSprite( &StaticTextSprite );
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
		return false;

	string font;
	int fontsize = 12;
	vector<int> vcolor;
	LuaConfig* cfg = new LuaConfig;
	//float textx, texty;
	//textx = texty = 0;

	cfg->getValue( "textx", config, "widget", TextX );
	cfg->getValue( "texty", config, "widget", TextY );
	cfg->getValue( "textalign", config, "widget", TextAlign );
	cfg->getValue( "font", config, "widget", font );
	cfg->getValue( "fontsize", config, "widget", fontsize );
	cfg->getValue( "fontcolor", config, "widget", vcolor );

	updatePosition( );

	setFont( font, fontsize );
	//setTextPosition( textx, texty );
	if( vcolor.size( ) > 2 )
		setFontColor(vcolor[0], vcolor[1], vcolor[2]);

	delete cfg;

	return true;
}

void TextWidget::updatePosition( )
{
	float posx, posy, swidth, width, height;
	Widget::updatePosition( );
	posx = posy = swidth = width = height = 0;
	if( StaticTextSprite ){
		swidth = StaticTextSprite->width;
		height = StaticTextSprite->height;
	}
	if( TextSprite ){
		width = TextSprite->width;
		if( !height || height < TextSprite->height )
			height = TextSprite->height;
	}
	switch(TextAlign){
		case CENTER:
			posx = PosX + this->Width * 0.5 - ( swidth + width ) * 0.5 + TextX;
			break;
		case RIGHT:
			posx = PosX + this->Width - ( swidth + width ) + TextX;
			break;
		case LEFT:
		default:
			posx = PosX + TextX;
			break;
	}
	posy = PosY - TextY;
	if( StaticTextSprite )
		StaticTextSprite->setPosition( posx, posy, getZ( ) + 0.1f );
	if( TextSprite )
		TextSprite->setPosition( posx + swidth, posy, getZ( ) + 0.1f );
}

void TextWidget::setFontColor( int r, int g, int b )
{
	if( StaticTextSprite )
		StaticTextSprite->clr->set( r, g, b );
	TextSprite->clr->set( r, g, b );
}

void TextWidget::setText( string text )
{
	float w, h;
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
	w = Width;
	h = Height;
	if( !Width || Width < TextSprite->width ){
		w = TextSprite->width;
		if( StaticTextSprite ){
			w += StaticTextSprite->width;
		}
	}
	if( !Height || Height < TextSprite->height ){
		h = TextSprite->height;
	}
	resize( w, h );
	updatePosition();
}

void TextWidget::setTextPosition( float x, float y )
{
	TextX = x;
	TextY = y;
	updatePosition();
}

void TextWidget::Update( )
{
	if( !Binded )
		return;
	if( (*Binded) != BindedCache ){
		BindedCache = (*Binded);
		char d[6];
		snprintf(d, 6, "%.0f", BindedCache);
		setText( d );
	}
}

void TextWidget::toggleVisibility( )
{
	Widget::toggleVisibility( );
	if( StaticTextSprite && StaticTextSprite->visible != visible )
		StaticTextSprite->toggleVisibility( );
	if( TextSprite && TextSprite->visible != visible )
		TextSprite->toggleVisibility( );
}

BarWidget::BarWidget()
{
	BarSprite = NULL;
	TopSprite = NULL;
	BarWidth = 0;
	BarMaxValue = 1;
	BindedMaxValue = NULL;
	BarX = 0;
	BarY = 0;
}

BarWidget::~BarWidget( )
{
	graph->FreeGLSprite( BarSprite );
	graph->FreeGLSprite( TopSprite );
}

bool BarWidget::load( string config )
{
	if( !TextWidget::load( config ) )
		return false;
	string imgname;
	int position[6];
	vector<int> vcolor;
	LuaConfig* cfg = new LuaConfig;

	//Order: topimgx, topimgy, barx, bary, barwidth, barheight
	//Ya, it's cruve, but it's simple
	cfg->getValue( "topimgx", config, "widget", position[0] );
	cfg->getValue( "topimgy", config, "widget", position[1] );
	cfg->getValue( "barx", config, "widget", BarX );
	cfg->getValue( "bary", config, "widget", BarY );
	cfg->getValue( "barwidth", config, "widget", BarWidth );
	cfg->getValue( "barheight", config, "widget", position[2] );
	cfg->getValue( "source", config, "widget", imgname );
	cfg->getValue( "barcolor", config, "widget", vcolor );
	if( vcolor.size( ) > 2 ){
		position[3] = vcolor[0]; //r
		position[4] = vcolor[1]; //g
		position[5] = vcolor[2]; //b
	}else{
		position[3] = position[4] = position[5] = 0;
	}
	if( BarWidth <= 0 )
		BarWidth = Width;
	createBar( imgname, position );
	updatePosition();

	delete cfg;

	return true;
}

void BarWidget::createBar( string name, int* pos)
{
	Texture* tex;
	tex = graph->LoadGLTexture( name );
	BarSprite = graph->CreateGLSprite( PosX + BarX, PosY + BarY, getZ(), BarWidth, pos[2] );
	if( tex )
		TopSprite = graph->CreateGLSprite( PosX, PosY, getZ() + 0.1f, pos[0], pos[1], Width, Height, tex );
	if( BarSprite ){
		BarSprite->clr->set( pos[3], pos[4], pos[5] );
	}
	setTextPosition( getTextX(), getTextY() - Height );
	setBarValue(1);
	setBarSize(1);
}

void BarWidget::setBarValue( float value )
{
	if( value == BarValue )
		return;
	BarValue = value;
	{//Output text;
		char str[25];
		snprintf( str, 25, "%.0f/%.0f", value, BarMaxValue );
		setText( str );
	}
	if( value < 0 )
		value = 0;
	if( value > BarMaxValue )
		value = BarMaxValue;
	if( BarSprite )
		BarSprite->resize( BarWidth * value / BarMaxValue, -1 );
}

void BarWidget::setBarSize( float val )
{
	if( val > 0 )
		BarMaxValue = val;
	else
		BarMaxValue = 1;
	{//Output text;
		char str[25];
		snprintf( str, 25, "%.0f/%.0f", BarValue, BarMaxValue );
		setText( str );
	}
	if( BarSprite )
		BarSprite->resize( BarWidth * BarValue / BarMaxValue, -1 );
}

void BarWidget::updatePosition( )
{
	TextWidget::updatePosition( );
	if( BarSprite )
		BarSprite->setPosition( PosX + BarX, PosY + BarY, getZ() );
	if( TopSprite )
		TopSprite->setPosition( PosX, PosY, getZ() + 0.01f );
}

bool BarWidget::bindBarMaxValue( float* val )
{
	if( val ){
		BindedMaxValue = val;
		return true;
	}
	return false;
}

void BarWidget::Update( )
{
	if( Binded != NULL ){
		if( (*Binded) != BarValue )
			setBarValue( *Binded );
	}
	if( BindedMaxValue != NULL ){
		if( (*BindedMaxValue) != BarMaxValue )
			setBarSize( *BindedMaxValue );
	}
}

void BarWidget::toggleVisibility( )
{
	TextWidget::toggleVisibility( );
	if( BarSprite && BarSprite->visible != visible )
		BarSprite->toggleVisibility( );
	if( TopSprite && TopSprite->visible != visible )
		TopSprite->toggleVisibility( );
}
