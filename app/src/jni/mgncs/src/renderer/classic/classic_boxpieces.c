
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/fixedmath.h>

#include "mgncsconfig.h"
#include "mcommon.h"
#include "mobject.h"
#include "mcomponent.h"
#include "mwidget.h"

#include "mrdr.h"
#include "piece.h"

///////////////////////////////////
//common fucntion
static void get_color_flag(DWORD *pcolor, int *pflag, int state, int check_state, mWidget *owner)
{
	DWORD fgcolor;
	int flag = 0;

	switch(state) //state
	{
	case NCS_ABP_NORMAL:
	case PIECE_STATE_CAPTURED:
		fgcolor = ncsGetElement(owner, NCS_BGC_3DBODY);
		flag |= NCSRS_NORMAL;
		break;
	case NCS_ABP_HILIGHT:
		fgcolor = ncsGetElement(owner, NCS_BGC_3DBODY);
		fgcolor = ncsCommRDRCalc3dboxColor(fgcolor, 100);
		//flag |= NCSRS_HIGHLIGHT;
		break;
	case NCS_ABP_PUSHED:
		fgcolor = ncsGetElement(owner, NCS_BGC_3DBODY);
		flag |= (NCSRF_INNERFRAME|NCSRS_SIGNIFICANT);
		break;
	case NCS_ABP_DISABLED:
		fgcolor = ncsGetElement(owner, NCS_BGC_DISABLED_ITEM);
		flag |= NCSRS_DISABLE;
		break;
    default:
        return;
	}

	switch(check_state)
	{
	case NCS_ABP_HALFCHECKED:
		//FIXED ME: select a right color
		fgcolor = ncsGetElement(owner, NCS_BGC_WINDOW);
	case NCS_ABP_CHECKED:
		flag |= (NCSRF_INNERFRAME|NCSRS_SIGNIFICANT);
		break;
	}

	*pcolor = fgcolor;
	*pflag = flag;
}

static void classic_draw_3dbox(HDC hdc, const RECT *rc, int flag ,int state, int check_state, mWidget *owner)
{
	DWORD fgcolor;
	int   flags;

	get_color_flag(&fgcolor, &flags, state, check_state, owner);
	flags |= flag;

	ncsCommRDRDraw3dbox(hdc, rc, fgcolor, flags);

}

static void classic_draw_flatbox(HDC hdc, const RECT *rc, int state, int check_state, mWidget *owner)
{
	DWORD fgcolor;
	int   flags = 0;

	get_color_flag(&fgcolor, &flags, state, check_state, owner);
	flags |= NCSRF_FILL;

	if(state == NCS_ABP_NORMAL)
	{
		gal_pixel old = SetBrushColor(hdc, ncsColor2Pixel(hdc, fgcolor));
		FillBox(hdc, rc->left, rc->top, RECTWP(rc), RECTHP(rc));
		SetBrushColor(hdc, old);
	}
	else
	{
		ncsCommRDRDraw3dbox(hdc, rc, fgcolor, flags);
	}

}

////////////////////////////////////////////
// button box
static void classic_buttonbox_paint(mButtonBoxPiece *self, HDC hdc, mWidget * owner, DWORD add_data)
{
	RECT rc;

	if(!_c(self)->getRect(self, &rc))
		return ;

	if(add_data & NCS_PIECE_PAINT_FLAT)
		classic_draw_flatbox(hdc, &rc, add_data&NCS_PIECE_PAINT_STATE_MASK,
			NCS_PIECE_PAINT_GET_CHECK(add_data), owner);
	else
		classic_draw_3dbox(hdc, &rc, NCSRF_FILL, add_data&NCS_PIECE_PAINT_STATE_MASK,
			NCS_PIECE_PAINT_GET_CHECK(add_data), owner);
}

//////////////////////////////////////////////////
// thumbbox
static void classic_thumbbox_paint(mThumbBoxPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
	BOOL bIsVert = add_data&0x00010000;
	RECT rc;
	int x, y, w, h;

	if(!_c(self)->getRect(self, &rc))
		return ;

	classic_draw_3dbox(hdc, &rc, NCSRF_FILL, add_data&NCS_PIECE_PAINT_STATE_MASK,
	NCS_PIECE_PAINT_GET_CHECK(add_data), owner);

	x = rc.left;
	y = rc.top;
	w = RECTW(rc);
	h = RECTH(rc);

	if (bIsVert) {
        MoveTo (hdc, x + (w>>1) - 4, y + (h>>1));
        LineTo (hdc, x + (w>>1) + 2, y + (h>>1));
        MoveTo (hdc, x + (w>>1) - 3, y + (h>>1) - 2);
        LineTo (hdc, x + (w>>1) + 1, y + (h>>1) - 2);
        MoveTo (hdc, x + (w>>1) - 3, y + (h>>1) + 2);
        LineTo (hdc, x + (w>>1) + 1, y + (h>>1) + 2);
    } else {
        MoveTo (hdc, x + (w>>1), y + (h>>1) - 4);
        LineTo (hdc, x + (w>>1), y + (h>>1) + 2);
        MoveTo (hdc, x + (w>>1) - 2, y + (h>>1) - 3);
        LineTo (hdc, x + (w>>1) - 2, y + (h>>1) + 1);
        MoveTo (hdc, x + (w>>1) + 2, y + (h>>1) - 3);
        LineTo (hdc, x + (w>>1) + 2, y + (h>>1) + 1);
    }
}

/////////////////////////////////////////////////////
//draw trackbar
static void classic_trackbar_paint(mTrackBarPiece* self, HDC hdc, mWidget *owner, DWORD add_data)
{
    /*
     * the data struct of add_data
     *
     * bits 0x0000FFFF  : the tick
     * bits 0x00FF0000  : horz or vert
     * bits 0xFF000000  : enable or disable
     */

	DWORD bgcolor;
	RECT  rc;
	fixed gap, tick;
	int   tick_count = add_data & 0xFFFF;

	if(!_c(self)->getRect(self, &rc))
		return;

	bgcolor = ncsGetElement(owner, NCS_BGC_3DBODY);

	if(!(NCS_PIECE_PAINT_VERT & add_data)) //horz
	{
		rc.top = (rc.top + rc.bottom)/2 - 3;
		rc.bottom = rc.top + 6;
		ncsCommRDRDraw3dbox(hdc, &rc, bgcolor,NCSRF_FILL | NCSRS_SIGNIFICANT);
		if(tick_count > 0)
		{
			gap = fixdiv(itofix(RECTW(rc)), itofix(tick_count));

			if((int)(fixtof(gap)) <= 0)
				return ;

			for(tick = itofix(rc.left);
				(int)(fixtof(tick)) <= rc.right; tick = fixadd(tick, gap))
			{
				MoveTo(hdc, (int)(fixtof(tick)), rc.bottom);
				LineTo(hdc, (int)(fixtof(tick)), rc.bottom + 4);
			}

			if((int)(fixtof(fixadd(fixsub(tick,gap), ftofix(0.9)))) < rc.bottom){
				MoveTo (hdc, rc.right, rc.bottom);
				LineTo (hdc, rc.right, rc.bottom + 4);
			}
		}
	} else //vert
	{
		rc.left = (rc.left + rc.right)/2 -3;
		rc.right = rc.left + 6;
		ncsCommRDRDraw3dbox(hdc, &rc, bgcolor,NCSRF_FILL | NCSRS_SIGNIFICANT);

		if(tick_count > 0)
		{
			gap = fixdiv(itofix(RECTH(rc)), itofix(tick_count));

			if((int)(fixtof(gap)) <= 0)
				return ;

			for(tick = itofix(rc.top);
				(int)(fixtof(tick)) <= rc.bottom; tick = fixadd(tick, gap))
			{
				MoveTo(hdc, rc.right, (int)(fixtof(tick)));
				LineTo(hdc, rc.right + 4, (int)(fixtof(tick)));
			}

			if((int)(fixtof(fixadd(fixsub(tick,gap), ftofix(0.9)))) < rc.bottom){
				MoveTo (hdc, rc.right, rc.bottom);
				LineTo (hdc, rc.right + 4, rc.bottom);
			}
		}
	}
}
////////////////////////////////////
//common check
static void classic_draw_check(HDC hdc, const RECT *rc, int flag ,int state, int check_state, mWidget *owner, int type)
{
    int w, h,  full_state=0;
    int box_l, box_t;
    const BITMAP* check_bmp = NULL;
	int style = 0;
	//int full_state = check_state + state;

//	if(add_data&0x10000)
	if(type == 0)
	{
		style = 13;
		check_bmp = GetSystemBitmapEx ("classic", SYSBMP_CHECKBUTTON);
		if(check_state)
			full_state = style*4 + (state<<3)+(state<<2) + state;
		else
			full_state = (state<<3) + (state<<2) + state;
	}
//	else if(add_data&0x100000)
	else if (type == 1)
	{
		style = 12;
		check_bmp = GetSystemBitmapEx ("classic", SYSBMP_RADIOBUTTON);
		if(check_state)
			full_state = style*4 + (state<<3)+(state<<2);
		else
			full_state = (state<<3) + (state<<2);

	}

	w = rc->right - rc->left;
    h = rc->bottom - rc->top;

	if( w >= style && h >= style){

		box_l = rc->left + (w>>1)-6;
		box_t = rc->top + (h>>1)-6;

		/*parity check*/
		if (w & 0x1)
			box_l += 1;

		if (h & 0x1)
			box_t += 1;

		FillBoxWithBitmapPart (hdc, box_l, box_t, style, style, 0, 0, check_bmp, full_state, 0);
	}
	else {
		FillBitmapPartInBox (hdc, rc->left, rc->top, w, h, check_bmp, full_state, 0, style, style);
	}
    return;
}

////////////////////////////////////
//checkbox
static void classic_checkbox_paint(mCheckBoxPiece* self, HDC hdc, mWidget *owner, DWORD add_data)
{
	RECT rc;

	if(!_c(self)->getRect(self, &rc))
		return ;

	classic_draw_check(hdc, &rc, NCSRF_FILL, add_data&NCS_PIECE_PAINT_STATE_MASK, NCS_PIECE_PAINT_GET_CHECK(add_data), owner, 0);
}
#if 1
//////////////////////////////////
//radiobox
static void classic_radiobox_paint(mCheckBoxPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
	RECT rc;

	if(!_c(self)->getRect(self, &rc))
		return ;

	classic_draw_check(hdc, &rc, NCSRF_FILL, add_data&NCS_PIECE_PAINT_STATE_MASK, NCS_PIECE_PAINT_GET_CHECK(add_data), owner, 1);
}
#endif
//////////////////////
#ifdef _MGNCSCTRL_SCROLLBAR
//scrollbar
static void classic_scrollbar_paint(mScrollBarPiece* self, HDC hdc, mWidget *owner, DWORD add_data)
{
	RECT rc;
	gal_pixel old_brush;
	DWORD barcolor;

	barcolor= ncsGetElement(owner, NCS_BGC_3DBODY);
	barcolor = ncsCommRDRCalc3dboxColor(barcolor, 15);
	old_brush = SetBrushColor(hdc, ncsColor2Pixel(hdc, barcolor));

	if(!_c(self)->getRect(self, &rc))
		return ;

	FillBox(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc));
	SetBrushColor(hdc, old_brush);
}

////////////////////////
//draw arrow
//
static void classic_arrow_paint(mScrollBarPiece *self, HDC hdc, mWidget * owner, DWORD add_data)
{
	RECT rc;
	DWORD arrow_color;
	int arrow;
	if(!_c(self)->getRect(self, &rc))
		return ;

	if(add_data & NCS_PIECE_PAINT_ARROW_SHELL)
	{
		classic_draw_3dbox(hdc, &rc, NCSRF_FILL, add_data&NCS_PIECE_PAINT_STATE_MASK,
					NCS_PIECE_PAINT_GET_CHECK(add_data), owner);
	}

	if((add_data&NCS_PIECE_PAINT_STATE_MASK) == PIECE_STATE_DISABLE)
		arrow_color = ncsGetElement(owner, NCS_FGC_DISABLED_ITEM);
	else
		arrow_color = ncsGetElement(owner, NCS_FGC_3DBODY);

	switch(add_data&NCS_PIECE_PAINT_ARROW_MASK)
	{
	case NCS_ARROWPIECE_RIGHT:
		arrow = NCSR_ARROW_RIGHT;
		break;
	case NCS_ARROWPIECE_UP:
		arrow = NCSR_ARROW_UP;
		break;
	case NCS_ARROWPIECE_DOWN:
		arrow = NCSR_ARROW_DOWN;
		break;
	case NCS_ARROWPIECE_LEFT:
	default:
		arrow = NCSR_ARROW_LEFT;
		break;
	}

	//draw arrow
	ncsCommRDRDrawArrow(hdc, &rc, arrow, arrow_color, NCSRF_FILL);
}
#endif //_MGNCSCTRL_SCROLLBAR

///////////////////////////////////////////
//grid box
// day grid
//

typedef void (*PGridBoxPaintContent)(mGridBoxPiece *self, HDC hdc, int row, int col, const RECT *c, mWidget *owner, void *paintData);

static void classic_grid_box_paint_all(mGridBoxPiece *self, HDC hdc, mWidget *owner, void *pPaintData,
	PGridBoxPaintContent paint_content)
{
	RECT rc, rc_cell;
	int cell_width, cell_height;
	int row, col;
	if(!_c(self)->getRect(self, &rc))
		return ;

	if(self->rows <= 0 || self->cols <= 0)
		return ;

	cell_width  = RECTW(rc) / self->cols;
	cell_height = RECTH(rc) / self->rows;

	rc_cell.top  = rc.top;

	for(row = 0; row < self->rows; row ++)
	{
		rc_cell.left = rc.left;
		if(row == self->rows - 1)
			rc_cell.bottom = rc.bottom;
		else
			rc_cell.bottom = rc_cell.top + cell_height;
		for(col = 0; col < self->cols; col ++)
		{
			if(col == self->cols - 1)
				rc_cell.right = rc.right;
			else
				rc_cell.right = rc_cell.left + cell_width;
			paint_content(self, hdc, row, col, &rc_cell, owner, pPaintData);
			rc_cell.left = rc_cell.right;
		}
		rc_cell.top = rc_cell.bottom;
	}
}

/////
//day grid

#define DAY_PREV  0
#define DAY_CUR   1
#define DAY_NEXT  2
static inline int get_day_by_cell(mDayGridPiece* self, int row, int col, int *pday)
{
	int day = _c(self)->cell2Day(self, row, col);

	if(day <= 0)
	{
		*pday = day + self->last_month_days;
		return DAY_PREV;
	}
	else if(day > self->month_days)
	{
		*pday = day - self->month_days;
		return DAY_NEXT;
	}

	*pday = day;

	return DAY_CUR;
}

typedef struct mDayGridPiecePaintContentData{
	mGridBoxPiecePaintData * paint_data;
	gal_pixel bkcolor;
	gal_pixel fgcolor;
	gal_pixel other_month_bkcolor;
	gal_pixel other_month_fgcolor;
	gal_pixel hot_bkcolor;
	gal_pixel hot_fgcolor;
	gal_pixel sel_bkcolor;
	gal_pixel sel_fgcolor;
	int last_type;
}mDayGridPiecePaintContentData;

static void classic_daygrid_paint_content(mDayGridPiece *self,
		HDC hdc,
		int row ,int col,
		const RECT *rc,
		mWidget *owner,
		mDayGridPiecePaintContentData *data)
{
	char szText[8];
	int day, daytype;

	daytype = get_day_by_cell(self, row, col, &day);

	if((data->paint_data->mask & NCS_GRIDBOXPIECE_PAINT_MASK_HOT)
			&& data->paint_data->hot_cell.row == row
			&& data->paint_data->hot_cell.col == col)
	{
		SetBrushColor(hdc, data->hot_bkcolor);
		SetTextColor(hdc, data->hot_fgcolor);
		data->last_type = -1;
	}
	else if((data->paint_data->mask & NCS_GRIDBOXPIECE_PAINT_MASK_DRAWSEL)
			&& data->paint_data->sel_cell.row == row
			&& data->paint_data->sel_cell.col == col)
	{
		SetBrushColor(hdc, data->sel_bkcolor);
		SetTextColor(hdc, data->sel_fgcolor);
		data->last_type = -1;
	}
	else if(daytype != data->last_type)
	{
		switch(daytype)
		{
		case DAY_PREV:
		case DAY_NEXT:
			SetBrushColor(hdc, data->other_month_bkcolor);
			SetTextColor(hdc, data->other_month_fgcolor);
			break;
		case DAY_CUR:
			SetBrushColor(hdc, data->bkcolor);
			SetTextColor(hdc, data->fgcolor);
			break;
		}
		data->last_type = daytype;
	}

	//fill RECT
	FillBox(hdc, rc->left, rc->top, RECTWP(rc), RECTHP(rc));

	//draw Text
	if(day>=10)
	{
		sprintf(szText, "%d", day);
	}
	else
	{
		sprintf(szText," %d", day);
	}


	DrawText(hdc, szText, 2, (RECT*)rc, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
}

static void classic_daygrid_paint(mDayGridPiece *self, HDC hdc, mWidget * owner, DWORD add_data)
{
	mGridBoxPiecePaintData * paint_data = (mGridBoxPiecePaintData*)add_data;
	mDayGridPiecePaintContentData paint_content_data;
	gal_pixel old_bkcolor;
	gal_pixel old_fgcolor;
	DWORD bkcolor;
	int mode;
	if(!paint_data)
	{
		return ;
	}

	mode = SetBkMode(hdc, BM_TRANSPARENT);

	paint_content_data.paint_data = paint_data;

	paint_content_data.last_type = -1;


	old_bkcolor = GetBrushColor(hdc);
	old_fgcolor = GetTextColor(hdc);


	if(paint_data->mask & NCS_GRIDBOXPIECE_PAINT_MASK_ALL_CELLS)
	{
		paint_content_data.bkcolor = ncsColor2Pixel(hdc, (bkcolor=ncsGetElement(owner, NCS_BGC_WINDOW)));
		paint_content_data.fgcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_FGC_WINDOW));

		paint_content_data.other_month_bkcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_BGC_DISABLED_ITEM));
		paint_content_data.other_month_fgcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_FGC_DISABLED_ITEM));


		paint_content_data.hot_bkcolor = ncsColor2Pixel(hdc, ncsCommRDRCalc3dboxColor(bkcolor, NCSR_COLOR_DARKER));
		paint_content_data.hot_fgcolor = paint_content_data.fgcolor;

		paint_content_data.sel_bkcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_BGC_SELECTED_ITEM));
		paint_content_data.sel_fgcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_FGC_SELECTED_ITEM));

		classic_grid_box_paint_all((mGridBoxPiece*)self, hdc, owner, &paint_content_data, (PGridBoxPaintContent)classic_daygrid_paint_content);
	}
	else
	{
		RECT rc;
		if(paint_data->mask & NCS_GRIDBOXPIECE_PAINT_MASK_DRAWSEL)
		{
			paint_content_data.sel_bkcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_BGC_SELECTED_ITEM));
			paint_content_data.sel_fgcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_FGC_SELECTED_ITEM));

			classic_daygrid_paint_content(self, hdc,
				paint_data->sel_cell.row, paint_data->sel_cell.col,
				_c(self)->getCellRect( self, paint_data->sel_cell.row, paint_data->sel_cell.col, &rc),
				owner,
				&paint_content_data);
		}
		else if(paint_data->mask & NCS_GRIDBOXPIECE_PAINT_MASK_REDRAW)
		{
			paint_content_data.bkcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_BGC_WINDOW));
			paint_content_data.fgcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_FGC_WINDOW));
			paint_content_data.other_month_bkcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_BGC_DISABLED_ITEM));
			paint_content_data.other_month_fgcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_FGC_DISABLED_ITEM));
			classic_daygrid_paint_content(self, hdc,
				paint_data->sel_cell.row, paint_data->sel_cell.col,
				_c(self)->getCellRect( self, paint_data->sel_cell.row, paint_data->sel_cell.col, &rc),
				owner,
				&paint_content_data);
		}
		else if(paint_data->mask & NCS_GRIDBOXPIECE_PAINT_MASK_HOT)
		{
			paint_content_data.hot_bkcolor = ncsColor2Pixel(hdc,
				ncsCommRDRCalc3dboxColor(ncsGetElement(owner, NCS_BGC_WINDOW),NCSR_COLOR_DARKER));
			paint_content_data.hot_fgcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_FGC_WINDOW));

			classic_daygrid_paint_content(self, hdc,
				paint_data->hot_cell.row, paint_data->hot_cell.col,
				_c(self)->getCellRect( self, paint_data->hot_cell.row, paint_data->hot_cell.col, &rc),
				owner,
				&paint_content_data);
		}
		else
			return ;
	}

	SetBrushColor(hdc, old_bkcolor);
	SetTextColor(hdc, old_fgcolor);
	SetBkMode(hdc, mode);
}


//////////////////////////////////////
//draw weekhead
static const char * default_week_names [] = {
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat",
	"Sun"
};
static void classic_weekhead_paint(mWeekHeadPiece *self, HDC hdc, mWidget * owner, DWORD add_data)
{
	RECT rc;
	int cell_size;
	int i;
	const char** weeknames = (const char**)self->week_names;
	DWORD bgcolor, fgcolor;
	gal_pixel old_bgcolor;
	gal_pixel old_fgcolor;
	if(weeknames == NULL)
		weeknames = default_week_names;

	if(!_c(self)->getRect(self, &rc))
		return ;

	cell_size = RECTW(rc)/7;

	bgcolor = ncsGetElement(owner, NCS_BGC_SELECTED_ITEM);
	fgcolor = ncsGetElement(owner, NCS_FGC_SELECTED_ITEM);

	old_bgcolor = SetBrushColor(hdc, ncsColor2Pixel(hdc, bgcolor));
	old_fgcolor = SetTextColor(hdc, ncsColor2Pixel(hdc, fgcolor));

	//Fill Box
	FillBox(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc));
	//draw Text
	for(i=0; i<7; i++)
	{
		rc.right = rc.left + cell_size;
		DrawText(hdc, weeknames[i], -1, &rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		rc.left = rc.right;
	}
	SetBrushColor(hdc, old_bgcolor);
	SetTextColor(hdc, old_fgcolor);
}


///////////////////////////////////////
//progresspiece
#ifdef _MGNCSCTRL_PROGRESSBAR
#define NCSPB_BLOCK 8
#define NCSPB_INNER 2
static void classic_progress_paint(mProgressPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
    gal_pixel old_color;
	RECT rc;
	RECT rc_prog;
	gal_pixel prog_color;
	int i;

	if(self->max <= self->min)
		return;

	if(!_c(self)->getRect(self, &rc))
		return;

	rc_prog = rc;
	//calc rc_prog;
	if(add_data & NCS_PIECE_PAINT_VERT)
	{
		int height = RECTH(rc_prog) * (self->cur - self->min) / (self->max - self->min);
		rc_prog.top = rc_prog.bottom - height;
	}
	else
	{
		int width = RECTW(rc_prog) * (self->cur - self->min) / (self->max - self->min);
		rc_prog.right = rc_prog.left + width;
	}

	if(IsRectEmpty(&rc_prog))
		return ;

	prog_color = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_BGC_HILIGHT_ITEM));

    old_color = SetBrushColor (hdc, prog_color);

	if(add_data & NCS_PIECE_PAINT_BLOCK)
	{
		if(add_data & NCS_PIECE_PAINT_VERT)
		{
			int width = RECTW(rc_prog);
			i=rc_prog.bottom;
			while(i > rc_prog.top)
			{
				FillBox(hdc, rc_prog.left, i - NCSPB_BLOCK, width, MIN(NCSPB_BLOCK, (i - rc_prog.top)));
				i -= (NCSPB_BLOCK+NCSPB_INNER);
			}
		}
		else
		{
			int height = RECTH(rc_prog);
			i = rc_prog.left;
			while(i < rc_prog.right)
			{
				FillBox(hdc, i, rc_prog.top, MIN(NCSPB_BLOCK, (rc_prog.right - i)), height);
				i += (NCSPB_BLOCK+NCSPB_INNER);
			}
		}
	}
	else
	{
		FillBox(hdc, rc_prog.left, rc_prog.top, RECTW(rc_prog), RECTH(rc_prog));
	}

	if(add_data & NCS_PIECE_PAINT_PERCENT)
	{
		RECT      rc_text;
		RECT      rc_tmp;
		int       percent = (self->cur - self->min)*100 / (self->max - self->min);
		char      szText[100];
		gal_pixel old_text_color;
		int       old_bkmode;
		SIZE      size;

		sprintf(szText, "%d%%", percent);
		GetTextExtent(hdc, szText, -1, &size);
		rc_text = rc;

		old_bkmode = SetBkMode(hdc, BM_TRANSPARENT);
		old_text_color = GetTextColor(hdc);

		if(add_data & NCS_PIECE_PAINT_VERT)
		{
			rc_text.top = (rc.top + rc.bottom - size.cy) / 2;
			rc_text.bottom = rc_text.top + size.cy;
			rc.bottom = rc_prog.top;
		}
		else
		{
			rc_text.left = (rc.left + rc.right - size.cx) / 2;
			rc_text.right = rc_text.left + size.cx;
			rc.left = rc_prog.right;
		}
		// draw
		if(IntersectRect(&rc_tmp, &rc_text, &rc_prog))
		{
			SelectClipRect(hdc, &rc_tmp);
			SetTextColor(hdc, GetWindowBkColor(owner->hwnd));
			DrawText(hdc, szText, -1, &rc_text, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}

		if(IntersectRect(&rc_tmp, &rc_text, &rc))
		{
			SelectClipRect(hdc, &rc_tmp);
			SetTextColor(hdc, prog_color);
			DrawText(hdc, szText, -1, &rc_text, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}

		SetTextColor(hdc, old_text_color);
		SetBkMode(hdc, old_bkmode);
	}

	SetBrushColor(hdc, old_color);
}
#endif

//////////////////////////////////
//init boxpiece
void classic_init_boxpiece_renderer(void)
{
	NCS_RDR_ENTRY entries [] = {
		{Class(mButtonBoxPiece).typeName, (mWidgetRenderer*)(void*)classic_buttonbox_paint},
		{Class(mThumbBoxPiece).typeName, (mWidgetRenderer*)(void*)classic_thumbbox_paint},
#ifdef _MGNCSCTRL_SCROLLBAR
		{Class(mScrollThumbBoxPiece).typeName, (mWidgetRenderer*)(void*)classic_thumbbox_paint},
#endif
		{Class(mTrackBarPiece).typeName, (mWidgetRenderer*)(void*)classic_trackbar_paint},
		{Class(mCheckBoxPiece).typeName, (mWidgetRenderer*)(void*)classic_checkbox_paint},
		{Class(mRadioBoxPiece).typeName, (mWidgetRenderer*)(void*)classic_radiobox_paint},
#ifdef _MGNCSCTRL_SCROLLBAR
		{Class(mScrollBarPiece).typeName, (mWidgetRenderer*)(void*)classic_scrollbar_paint},
		{Class(mLeftArrowPiece).typeName, (mWidgetRenderer*)(void*)classic_arrow_paint},
		{Class(mRightArrowPiece).typeName, (mWidgetRenderer*)(void*)classic_arrow_paint},
		{Class(mUpArrowPiece).typeName, (mWidgetRenderer*)(void*)classic_arrow_paint},
		{Class(mDownArrowPiece).typeName, (mWidgetRenderer*)(void*)classic_arrow_paint},
#endif
		{Class(mDayGridPiece).typeName, (mWidgetRenderer*)(void*)classic_daygrid_paint},
		{Class(mWeekHeadPiece).typeName, (mWidgetRenderer*)(void*)classic_weekhead_paint},
#ifdef _MGNCSCTRL_PROGRESSBAR
		{Class(mProgressPiece).typeName, (mWidgetRenderer*)(void*)classic_progress_paint}
#endif
		};

	ncsRegisterCtrlRDRs("classic",
		entries,
		sizeof(entries)/sizeof(NCS_RDR_ENTRY));

}

