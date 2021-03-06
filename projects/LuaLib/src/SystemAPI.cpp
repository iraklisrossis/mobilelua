/*
 * Copyright (c) 2011 MoSync AB
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <ma.h>
#include <maheap.h>
#include <mastring.h>
#include <mawstring.h>
#include <conprint.h>
#include <MAUtil/String.h>
#include <MAUtil/Geometry.h>
#include <MAUI/Font.h>
#include <MAUI/Font.h>
#include <IX_WIDGET.h>

extern "C" {
#include "inc/SystemAPI.h"
}

/**********************************************************
 * Helper classes used locally in this file
 **********************************************************/

/**
 * C++ classes go in the MobileLua namespace.
 * C-functions are in the global namespace.
 */
namespace MobileLua
{

class TextObject
{
public:
	MAUI::Font* font;
	MAUtil::String text;

	TextObject()
	{
		// Created font lazily when needed.
		font = NULL;
	}

	virtual ~TextObject()
	{
		if (NULL != font)
		{
			delete font;
			font = NULL;
		}
	}

	MAUI::Font* getFont()
	{
		return font;
	}

	const char* getString()
	{
		return text.c_str();
	}
};

} // namespace MobileLua


/**********************************************************
 * Functions for text and font handling
 **********************************************************/

extern "C" void* SysTextCreate(int fontHandle)
{
	MobileLua::TextObject* textObj = new MobileLua::TextObject();
	textObj->font = new MAUI::Font(fontHandle);
	return (void*) textObj;
}

extern "C" void SysTextDelete(void* textObj)
{
	MobileLua::TextObject* self = (MobileLua::TextObject*)textObj;
	delete self;
}

extern "C" void SysTextSetString(void* textObj, char* str)
{
	MobileLua::TextObject* self = (MobileLua::TextObject*)textObj;

	self->text = str;
}

extern "C" void SysTextSetLineSpacing(void* textObj, int lineSpacing)
{
	MobileLua::TextObject* self = (MobileLua::TextObject*)textObj;

	self->font->setLineSpacing(lineSpacing);
}

extern "C" int SysTextGetStringSize(void* textObj, int extent)
{
	MobileLua::TextObject* self = (MobileLua::TextObject*)textObj;

	if (0 == extent)
	{
		return self->font->getStringDimensions(
			self->getString());
	}
	else
	{
		MAUtil::Rect bounds(0, 0, EXTENT_X(extent), EXTENT_Y(extent));
		return self->font->getBoundedStringDimensions(
			self->getString(),
			bounds);
	}
}

extern "C" void SysTextDrawString(void* textObj, int x, int y, int extent)
{
	MobileLua::TextObject* self = (MobileLua::TextObject*)textObj;

	if (0 == extent)
	{
		return self->font->drawString(
			self->getString(),
			x,
			y);
	}
	else
	{
		MAUtil::Rect bounds(0, 0, EXTENT_X(extent), EXTENT_Y(extent));
		return self->font->drawBoundedString(
			self->getString(),
			x,
			y,
			bounds);
	}
}

/**********************************************************
 * Functions for using C memory from Lua
 **********************************************************/

/**
 * Allocate a memory block.
 */
extern "C" void* SysAlloc(int size)
{
	return malloc(size);
}

/**
 * Use this function to free allocated data structures.
 */
extern "C" void SysFree(void* buffer)
{
	free(buffer);
}

/**
 * Get an int value in a memory block.
 * @param buffer Pointer to memory block.
 * @param index Offset to an integer index (as if the
 * memory block was an array of ints).
 * @return The int at the given index.
 */
extern "C" int SysBufferGetInt(void* buffer, int index)
{
	int* p = (int*) buffer;
	return p[index];
}

/**
 * Set an int value in a memory block.
 * @param buffer Pointer to memory block.
 * @param index Offset to an integer index (as if the
 * memory block was an array of ints).
 */
extern "C" void SysBufferSetInt(void* buffer, int index, int value)
{
	int* p = (int*) buffer;
	p[index] = value;
}

/**
 * Get a byte value in a memory block.
 * @param buffer Pointer to memory block.
 * @param index Offset to a byte index (as if the
 * memory block was an array of bytes).
 * @return The byte value at the given index.
 */
extern "C" int SysBufferGetByte(void* buffer, int index)
{
	byte* p = (byte*) buffer;
	return (int) (p[index]);
}

/**
 * Set a byte value in a memory block.
 * @param buffer Pointer to memory block.
 * @param index Offset to a byte index (as if the
 * memory block was an array of bytes).
 */
extern "C" void SysBufferSetByte(void* buffer, int index, int value)
{
	byte* p = (byte*) buffer;
	p[index] = (byte) value;
}

/**
 * Get an float value in a memory block.
 * @param buffer Pointer to memory block.
 * @param index Offset to a float index (as if the
 * memory block was an array of floats).
 * @return The float value at the given index.
 */
extern "C" float SysBufferGetFloat(void* buffer, int index)
{
	float* p = (float*) buffer;
	return p[index];
}

/**
 * Get an double value in a memory block.
 * @param buffer Pointer to memory block.
 * @param index Offset to a double index (as if the
 * memory block was an array of doubles).
 * @return The double value at the given index.
 */
extern "C" double SysBufferGetDouble(void* buffer, int index)
{
	double* p = (double*) buffer;
	return p[index];
}

/**
 * Copy bytes from one memory block to another. The number of bytes
 * given by numberOfBytesToCopy bytes, starting at sourceIndex in 
 * the source block, will be copied to the destination block, 
 * starting at destIndex.
 * @param sourceBuffer Pointer to the source memory block.
 * @param sourceIndex Offset to a byte index in the source block.
 * @param destBuffer Pointer to the destination memory block.
 * @param destIndex Offset to a byte index in the destination block.
 * @param numberOfBytesToCopy Number of bytes that will be copied
 * from source to destination.
 */
extern "C" void SysBufferCopyBytes(
	void* sourceBuffer, 
	int sourceIndex, 
	void* destBuffer, 
	int destIndex, 
	int numberOfBytesToCopy)
{
	byte* source = (byte*) sourceBuffer;
	byte* dest = (byte*) destBuffer;
	
	// Not the very fastest implementation perhaps...
	for (int i = 0; i < numberOfBytesToCopy; ++i)
	{
		dest[destIndex + i] = source[sourceIndex + i];
	}
}

/**
 * Return a pointer to a byte at an index in a buffer.
 * This function is useful when calling functions that
 * write to memory using a pointer, or to read data
 * from a particular place in a memory block.
 * @param buffer Pointer to  memory block.
 * @param index Offset to a byte index.
 * @return A pointer to the byte at the given index.
 */
void* SysBufferGetBytePointer(void* buffer, int index)
{
	return ((char*) buffer) + index;
}

/**********************************************************
 * Functions for getting the size of C data types
 **********************************************************/

/**
 * Get the size of an int in bytes.
 * @return The size.
 */
extern "C" int SysSizeOfInt()
{
	return (int) sizeof(int);
}

/**
 * Get the size of an int in bytes.
 * @return The size.
 */
extern "C" int SysSizeOfFloat()
{
	return (int) sizeof(float);
}

/**
 * Get the size of an int in bytes.
 * @return The size.
 */
extern "C" int SysSizeOfDouble()
{
	return (int) sizeof(double);
}

/**********************************************************
 * Functions for bit operations and bit-shift
 **********************************************************/

extern "C" int SysBitAnd(int a, int b)
{
	return ((unsigned int) a) & ((unsigned int) a);
}

extern "C" int SysBitOr(int a, int b)
{
	return ((unsigned int) a) | ((unsigned int) a);
}

extern "C" int SysBitXor(int a, int b)
{
	return ((unsigned int) a) ^ ((unsigned int) a);
}

extern "C" int SysBitNot(int a)
{
	return ~((unsigned int) a);
}

extern "C" int SysBitShiftLeft(int a, int bits)
{
	return ((unsigned int) a) << bits;
}

extern "C" int SysBitShiftRight(int a, int bits)
{
	return ((unsigned int) a) >> bits;
}

/**********************************************************
 * Event functions
 **********************************************************/

extern "C" MAEvent* SysEventCreate()
{
	return (MAEvent*) SysAlloc(sizeof(MAEvent));
}

extern "C" int SysEventGetType(MAEvent* event)
{
	return event->type;
}

extern "C" int SysEventGetKey(MAEvent* event)
{
	return event->key;
}

extern "C" int SysEventGetNativeKey(MAEvent* event)
{
	return event->nativeKey;
}

extern "C" uint SysEventGetCharacter(MAEvent* event)
{
	return event->character;
}

extern "C" int SysEventGetX(MAEvent* event)
{
	return event->point.x;
}

extern "C" int SysEventGetY(MAEvent* event)
{
	return event->point.y;
}

extern "C" int SysEventGetTouchId(MAEvent* event)
{
	return event->touchId;
}

extern "C" int SysEventGetState(MAEvent* event)
{
	return event->state;
}

extern "C" MAHandle SysEventGetConnHandle(MAEvent* event)
{
	return event->conn.handle;
}

extern "C" int SysEventGetConnOpType(MAEvent* event)
{
	return event->conn.opType;
}

extern "C" int SysEventGetConnResult(MAEvent* event)
{
	return event->conn.result;
}

extern "C" int SysEventGetTextBoxResult(MAEvent* event)
{
	return event->textboxResult;
}

extern "C" int SysEventGetTextBoxLength(MAEvent* event)
{
	return event->textboxLength;
}

extern "C" void* SysEventGetData(MAEvent* event)
{
	return event->data;
}

extern "C" int SysEventSensorGetType(MAEvent* event)
{
	return event->sensor.type;
}

extern "C" float SysEventSensorGetValue1(MAEvent* event)
{
	return event->sensor.values[0];
}

extern "C" float SysEventSensorGetValue2(MAEvent* event)
{
	return event->sensor.values[1];
}

extern "C" float SysEventSensorGetValue3(MAEvent* event)
{
	return event->sensor.values[2];
}

extern "C" int SysEventLocationGetState(MAEvent* event)
{
	return ((MALocation*)(event->data))->state;
}

extern "C" double SysEventLocationGetLat(MAEvent* event)
{
	return ((MALocation*)(event->data))->lat;
}

extern "C" double SysEventLocationGetLon(MAEvent* event)
{
	return ((MALocation*)(event->data))->lon;
}

extern "C" double SysEventLocationGetHorzAcc(MAEvent* event)
{
	return ((MALocation*)(event->data))->horzAcc;
}

extern "C" double SysEventLocationGetVertAcc(MAEvent* event)
{
	return ((MALocation*)(event->data))->vertAcc;
}

extern "C" float SysEventLocationGetAlt(MAEvent* event)
{
	return ((MALocation*)(event->data))->alt;
}

extern "C" int SysWidgetEventGetType(void* widgetEvent)
{
	return ((MAWidgetEventData*)widgetEvent)->eventType;
}

extern "C" int SysWidgetEventGetHandle(void* widgetEvent)
{
	return ((MAWidgetEventData*)widgetEvent)->widgetHandle;
}

extern "C" int SysWidgetEventGetListItemIndex(void* widgetEvent)
{
	return ((MAWidgetEventData*)widgetEvent)->listItemIndex;
}

extern "C" int SysWidgetEventGetChecked(void* widgetEvent)
{
	return ((MAWidgetEventData*)widgetEvent)->checked;
}

extern "C" int SysWidgetEventGetTabIndex(void* widgetEvent)
{
	return ((MAWidgetEventData*)widgetEvent)->tabIndex;
}

extern "C" int SysWidgetEventGetUrlData(void* widgetEvent)
{
	return ((MAWidgetEventData*)widgetEvent)->urlData;
}

/**********************************************************
 * Point functions
 **********************************************************/

extern "C" MAPoint2d* SysPointCreate()
{
	return (MAPoint2d*) SysAlloc(sizeof(MAPoint2d));
}

extern "C" int SysPointGetX(MAPoint2d* point)
{
	return point->x;
}

extern "C" int SysPointGetY(MAPoint2d* point)
{
	return point->y;
}

extern "C" void SysPointSetX(MAPoint2d* point, int x)
{
	point->x = x;
}

extern "C" void SysPointSetY(MAPoint2d* point, int y)
{
	point->y = y;
}

/**********************************************************
 * Rect functions
 **********************************************************/

extern "C" MARect* SysRectCreate()
{
	return (MARect*) SysAlloc(sizeof(MARect));
}

extern "C" int SysRectGetLeft(MARect* rect)
{
	return rect->left;
}

extern "C" int SysRectGetTop(MARect* rect)
{
	return rect->top;
}

extern "C" int SysRectGetWidth(MARect* rect)
{
	return rect->width;
}

extern "C" int SysRectGetHeight(MARect* rect)
{
	return rect->height;
}

extern "C" void SysRectSetLeft(MARect* rect, int left)
{
	rect->left = left;
}

extern "C" void SysRectSetTop(MARect* rect, int top)
{
	rect->top = top;
}

extern "C" void SysRectSetWidth(MARect* rect, int width)
{
	rect->width = width;
}

extern "C" void SysRectSetHeight(MARect* rect, int height)
{
	rect->height = height;
}

/**********************************************************
 * Misc functions and string functions
 **********************************************************/

extern "C" MACopyData* SysCopyDataCreate(
	MAHandle dst, int dstOffset, MAHandle src, int srcOffset, int size)
{
	MACopyData* data = (MACopyData*) SysAlloc(sizeof(MACopyData));
	if (NULL != data)
	{
		data->dst = dst;
		data->dstOffset = dstOffset;
		data->src = src;
		data->srcOffset = srcOffset;
		data->size = size;
	}
	return data;
}

extern "C" void SysScreenSetColor(int red, int green, int blue)
{
	int color = (red << 16) | (green << 8) | blue;
	maSetColor(color);
}

extern "C" void SysScreenDrawText(const char* text, int x, int y, void* font)
{
	MAUI::Font* pFont = (MAUI::Font*) font;
	if (NULL == pFont)
	{
		lprintfln("SysScreenDrawText: Font not found");
		return;
	}

	lprintfln("SysScreenDrawText: Drawing text: %s", text);

	// Draw string.
	MAUtil::Rect bounds(0, 0, EXTENT_X(maGetScrSize()), EXTENT_Y(maGetScrSize()));
	pFont->drawBoundedString(text, x, y, bounds);
}

/**
 * Convert a char string to a wchar string.
 * It is the responsibility of the caller to deallocate the
 * returned string with SysBufferDelete.
 */
extern "C" void* SysStringCharToWideChar(const char* str)
{
	if (NULL == str)
	{
		return NULL;
	}

	// Allocate result string.
	int length = strlen(str);
	wchar* wstr = (wchar*) SysAlloc((1 + length) * sizeof(wchar));
	if (NULL == wstr)
	{
		return NULL;
	}

	// Copy string.
	int i;
	for (i = 0; i < length; ++i)
	{
		wstr[i] = str[i];
	}

	// Zero terminate string.
	wstr[i] = 0;

	return wstr;
}

/**
 * Convert a wchar string to a char string.
 * In C it is the responsibility of the caller to deallocate the
 * returned string with SysFree. When called from Lua, a Lua string
 * will be returned.
 * Conversion only supports basic 256 char set.
 */
extern "C" char* SysStringWideCharToChar(const void* wstr)
{
	if (NULL == wstr)
	{
		return NULL;
	}

	// Allocate result string.
	int length = wcslen((wchar*)wstr);
	char* str = (char*) SysAlloc((1 + length) * sizeof(char));
	if (NULL == str)
	{
		return NULL;
	}

	// Copy string.
	int i;
	for (i = 0; i < length; ++i)
	{
		// Brute force conversion!
		str[i] = (char) ((wchar*)wstr)[i];
	}

	// Zero terminate string.
	str[i] = 0;

	return str;
}

/**
 * Helper method that reads a text string from resource file.
 * In C it is the responsibility of the caller to deallocate the
 * returned string with SysFree. When called from Lua, a Lua string
 * will be returned.
 */
extern "C" char* SysLoadStringResource(MAHandle data)
{
	// Get size of data.
    int size = maGetDataSize(data);

    // Allocate space for text plus zero termination character.
    char* text = (char*) SysAlloc(size + 1);
    if (NULL == text)
    {
    	return NULL;
    }

    // Read data.
    maReadData(data, text, 0, size);

    // Zero terminate string.
    text[size] = 0;

    return text;
}

