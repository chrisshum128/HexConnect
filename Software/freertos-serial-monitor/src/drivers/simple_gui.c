/**
 ******************************************************************************
 * @file	simple_gui.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2014-06-14
 * @brief
 ******************************************************************************
	Copyright (c) 2014 Hampus Sandberg.

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation, either
	version 3 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "simple_gui.h"

/* Private defines -----------------------------------------------------------*/
/* Private typedefs ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static GUIButton prvButton_list[guiConfigNUMBER_OF_BUTTONS];
static GUITextBox prvTextBox_list[guiConfigNUMBER_OF_TEXT_BOXES];
static GUIContainer prvContainer_list[guiConfigNUMBER_OF_CONTAINERS];

static const uint8_t prvHexTable[16] = {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

/* Private function prototypes -----------------------------------------------*/
static int32_t prvItoa(int32_t Number, uint8_t* Buffer);
static GUILayer prvCurrentlyActiveLayer;
static bool prvBeepIsOn = true;

/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the GUI by setting the items in the lists to appropriate values
 * @param	None
 * @retval	None
 */
void GUI_Init()
{
	/* Buttons */
	for (uint32_t i = 0; i < guiConfigNUMBER_OF_BUTTONS; i++)
	{
		memset(&prvButton_list[i], 0, sizeof(GUIButton));
	}

	/* Text boxes */
	for (uint32_t i = 0; i < guiConfigNUMBER_OF_TEXT_BOXES; i++)
	{
		memset(&prvTextBox_list[i], 0, sizeof(GUITextBox));
	}

	/* Pages */
	for (uint32_t i = 0; i < guiConfigNUMBER_OF_CONTAINERS; i++)
	{
		memset(&prvContainer_list[i], 0, sizeof(GUIContainer));
	}
}

/**
 * @brief	Draw the border on an object
 * @param	Object: The object to draw for
 * @retval	None
 */
void GUI_DrawBorder(GUIObject Object)
{
	/* Draw the border */
	LCD_SetForegroundColor(Object.borderColor);
	if (Object.border & GUIBorder_Left)
	{
		LCD_DrawSquareOrLine(Object.xPos, Object.xPos + Object.borderThickness - 1,
							 Object.yPos, Object.yPos + Object.height - 1,
							 LCDDrawType_Square, LCDFill_Fill);
	}
	if (Object.border & GUIBorder_Right)
	{
		LCD_DrawSquareOrLine(Object.xPos + Object.width - Object.borderThickness, Object.xPos + Object.width - 1,
							 Object.yPos, Object.yPos + Object.height - 1,
							 LCDDrawType_Square, LCDFill_Fill);
	}
	if (Object.border & GUIBorder_Top)
	{
		LCD_DrawSquareOrLine(Object.xPos, Object.xPos + Object.width - 1,
							 Object.yPos, Object.yPos + Object.borderThickness - 1,
							 LCDDrawType_Square, LCDFill_Fill);
	}
	if (Object.border & GUIBorder_Bottom)
	{
		LCD_DrawSquareOrLine(Object.xPos, Object.xPos + Object.width - 1,
							 Object.yPos + Object.height - Object.borderThickness, Object.yPos + Object.height - 1,
							 LCDDrawType_Square, LCDFill_Fill);
	}
}

/**
 * @brief	Redraw all objects on a specified layer
 * @param	Layer: The layer to redraw
 * @retval	None
 */
void GUI_RedrawLayer(GUILayer Layer)
{
	/* Buttons */
	for (uint32_t i = 0; i < guiConfigNUMBER_OF_BUTTONS; i++)
	{
		if (prvButton_list[i].object.layer == Layer && prvButton_list[i].object.displayState != GUIDisplayState_Hidden)
		{
			GUI_DrawButton(prvButton_list[i].object.id);
		}
	}

	/* Text Boxes */
	for (uint32_t i = 0; i < guiConfigNUMBER_OF_TEXT_BOXES; i++)
	{
		if (prvTextBox_list[i].object.layer == Layer && prvTextBox_list[i].object.displayState != GUIDisplayState_Hidden)
		{
			GUITextBox_Draw(prvTextBox_list[i].object.id);
		}
	}
}

/**
 * @brief	Sets the layer given as the active layer
 * @param	Layer: The layer to set to
 * @retval	None
 * @note	Layers below this layer won't be drawn until the layer is lowered to their level
 */
void GUI_SetActiveLayer(GUILayer Layer)
{
	prvCurrentlyActiveLayer = Layer;
}

/**
 * @brief	Get the currently active layer
 * @param	None
 * @retval	The currently active layer
 */
GUILayer GUI_GetActiveLayer()
{
	return prvCurrentlyActiveLayer;
}

/**
 * @brief	Turn on beep
 * @param	None
 * @retval	None
 */
void GUI_SetBeepOn()
{
	prvBeepIsOn = true;
}

/**
 * @brief	Turn off beep
 * @param	None
 * @retval	None
 */
void GUI_SetBeepOff()
{
	prvBeepIsOn = false;
}

/**
 * @brief	Get the current state of the beep
 * @param	None
 * @retval	The value of prvBeepIsOn
 */
bool GUI_BeepIsOn()
{
	return prvBeepIsOn;
}

/* Button --------------------------------------------------------------------*/
/**
 * @brief	Get a pointer to the button corresponding to the id
 * @param	ButtonId: The id of the button to get
 * @retval	Pointer the button or 0 if no button was found
 */
GUIButton* GUI_GetButtonFromId(uint32_t ButtonId)
{
	uint32_t index = ButtonId - guiConfigBUTTON_ID_OFFSET;

	if (index < guiConfigNUMBER_OF_BUTTONS)
		return &prvButton_list[index];
	else
		return 0;
}
/**
 * @brief	Add a button to the button list
 * @param	Button: Pointer to a GUIButton_TypeDef struct which data should be copied from
 * @retval	SUCCESS if everything went OK
 * @retval	ERROR: if something went wrong
 */
ErrorStatus GUI_AddButton(GUIButton* Button)
{
	uint32_t index = Button->object.id - guiConfigBUTTON_ID_OFFSET;
	ErrorStatus status;

	/* Make sure we don't try to create more button than there's room for in the button_list */
	if (index < guiConfigNUMBER_OF_BUTTONS)
	{
		/* Copy the button to the list */
		memcpy(&prvButton_list[index], Button, sizeof(GUIButton));

		/* Get a pointer to the current button */
		GUIButton* button = &prvButton_list[index];

		/* Two rows of text */
		if (button->text[0] != 0 && button->text[1] != 0)
		{
			button->numOfChar[0] = strlen(button->text[0]);
			button->textWidth[0] = button->numOfChar[0] * guiConfigFONT_WIDTH_UNIT * button->textSize[0];
			button->textHeight[0] = guiConfigFONT_HEIGHT_UNIT * button->textSize[0];

			button->numOfChar[1] = strlen(button->text[1]);
			button->textWidth[1] = button->numOfChar[1] * guiConfigFONT_WIDTH_UNIT * button->textSize[1];
			button->textHeight[1] = guiConfigFONT_HEIGHT_UNIT * button->textSize[1];
		}
		/* One row of text */
		else
		{
			/* Fill in the rest of the data */
			button->numOfChar[0] = strlen(button->text[0]);
			button->textWidth[0] = button->numOfChar[0] * guiConfigFONT_WIDTH_UNIT * button->textSize[0];
			button->textHeight[0] = guiConfigFONT_HEIGHT_UNIT * button->textSize[0];
		}

		/* If it's set to not hidden we should draw the button */
		if (Button->object.displayState == GUIDisplayState_NotHidden)
			status = GUI_DrawButton(Button->object.id);
	}
	else
		status = ERROR;

	/* Set all the data in the Button we received as a parameter to 0 so that it can be reused easily */
	memset(Button, 0, sizeof(GUIButton));

	return status;
}


/**
 * @brief	Hides a button by drawing a black box over it
 * @param	ButtonId: The id of the button to hide
 * @retval	None
 */
void GUI_HideButton(uint32_t ButtonId)
{
	uint32_t index = ButtonId - guiConfigBUTTON_ID_OFFSET;

	/* Make sure the index is valid and that the correct layer is active */
	if (index < guiConfigNUMBER_OF_BUTTONS && prvButton_list[index].object.layer == prvCurrentlyActiveLayer)
	{
		/* Get a pointer to the current button */
		GUIButton* button = &prvButton_list[index];

		/* Set the background color */
		LCD_SetBackgroundColor(LCD_COLOR_BLACK);
		/* Clear the active window */
		LCDActiveWindow window;
		window.xLeft = button->object.xPos;
		window.xRight = button->object.xPos + button->object.width - 1;
		window.yTop = button->object.yPos;
		window.yBottom = button->object.yPos + button->object.height - 1;
		LCD_ClearActiveWindow(window.xLeft, window.xRight, window.yTop, window.yBottom);
		button->object.displayState = GUIDisplayState_Hidden;
	}
}

/**
 * @brief	Draw a specific button in the button_list
 * @param	None
 * @retval	SUCCESS if everything went OK
 * @retval	ERROR: if something went wrong
 */
ErrorStatus GUI_DrawButton(uint32_t ButtonId)
{
	uint32_t index = ButtonId - guiConfigBUTTON_ID_OFFSET;

	/* Make sure the index is valid and that the correct layer is active */
	if (index < guiConfigNUMBER_OF_BUTTONS && prvButton_list[index].text != 0 && prvButton_list[index].object.layer == prvCurrentlyActiveLayer)
	{
		/* Get a pointer to the current button */
		GUIButton* button = &prvButton_list[index];

		/* Set state colors */
		uint16_t backgroundColor, textColor;
		switch (button->state) {
			case GUIButtonState_Enabled:
				/* Enabled state */
				backgroundColor = button->enabledBackgroundColor;
				textColor = button->enabledTextColor;
				break;
			case GUIButtonState_Disabled:
			case GUIButtonState_DisabledTouch:
				/* Disabled state */
				backgroundColor = button->disabledBackgroundColor;
				textColor = button->disabledTextColor;
				break;
			case GUIButtonState_TouchDown:
				/* Disabled state */
				backgroundColor = button->pressedBackgroundColor;
				textColor = button->pressedTextColor;
				break;
			default:
				break;
		}

		/* Draw the button */
		LCD_SetForegroundColor(backgroundColor);
		LCD_DrawSquareOrLine(button->object.xPos, button->object.xPos + button->object.width-1,
							 button->object.yPos, button->object.yPos + button->object.height-1, LCDDrawType_Square, LCDFill_Fill);
		LCD_SetForegroundColor(textColor);

		/* Two rows of text */
		if (button->text[0] != 0 && button->text[1] != 0)
		{
			uint16_t xPos = button->object.xPos + (button->object.width - button->textWidth[0]) / 2;
			uint16_t yPos = button->object.yPos + button->object.height/2 - button->textHeight[0] - 1;
			LCD_SetTextWritePosition(xPos, yPos);
			LCD_WriteString(button->text[0], LCDTransparency_Transparent, button->textSize[0]);

			xPos = button->object.xPos + (button->object.width - button->textWidth[1]) / 2;
			yPos = button->object.yPos + button->object.height/2 + 1;
			LCD_SetTextWritePosition(xPos, yPos);
			LCD_WriteString(button->text[1], LCDTransparency_Transparent, button->textSize[1]);
		}
		/* One row of text */
		else
		{
			uint16_t xPos = button->object.xPos + (button->object.width - button->textWidth[0]) / 2;
			uint16_t yPos = button->object.yPos + (button->object.height - button->textHeight[0]) / 2 - 2;
			LCD_SetTextWritePosition(xPos, yPos);
			LCD_WriteString(button->text[0], LCDTransparency_Transparent, button->textSize[0]);
		}

		/* Draw the border */
		GUI_DrawBorder(button->object);

		button->object.displayState = GUIDisplayState_NotHidden;

		return SUCCESS;
	}
	else
		return ERROR;
}

/**
 * @brief	Draw all buttons in the button_list
 * @param	None
 * @retval	None
 */
void GUI_DrawAllButtons()
{
	for (uint32_t i = 0; i < guiConfigNUMBER_OF_BUTTONS; i++)
	{
		GUI_DrawButton(i);
	}
}

/**
 * @brief	Set the state of the button
 * @param	ButtonId: The Id for the button
 * @param	State: The new state
 * @retval	None
 */
void GUI_SetButtonState(uint32_t ButtonId, GUIButtonState State)
{
	uint32_t index = ButtonId - guiConfigBUTTON_ID_OFFSET;
	if (index < guiConfigNUMBER_OF_BUTTONS)
	{
		prvButton_list[index].state = State;
		GUI_DrawButton(index);
	}
}

/**
 * @brief	Set the state of the button
 * @param	ButtonId: The Id for the button
 * @param	Text: The text to set
 * @param	Row: The row to set, can be 0 or 1
 * @retval	None
 * @warning Make sure the Text is smaller than or equal to the text set during init
 */
void GUI_SetButtonTextForRow(uint32_t ButtonId, uint8_t* Text, uint32_t Row)
{
	uint32_t index = ButtonId - guiConfigBUTTON_ID_OFFSET;
	if (index < guiConfigNUMBER_OF_BUTTONS && Row < 2)
	{
		GUIButton* button = &prvButton_list[index];
		/* Save the new button */
		button->text[Row] = Text;

		/* Update the size variables */
		button->numOfChar[Row] = strlen(button->text[Row]);
		button->textWidth[Row] = button->numOfChar[Row] * guiConfigFONT_WIDTH_UNIT * button->textSize[Row];
		button->textHeight[Row] = guiConfigFONT_HEIGHT_UNIT * button->textSize[Row];

		/* Draw the button so that the changes appear */
		if (button->object.displayState == GUIDisplayState_NotHidden)
			GUI_DrawButton(ButtonId);
	}
}

/**
 * @brief	Check if a button is located at the position where a touch up event occurred
 * @param	GUITouchEvent: The event that happened, can be any value of GUITouchEvent
 * @param	XPos: X-position for event
 * @param	XPos: Y-position for event
 * @retval	None
 */
void GUI_CheckAllActiveButtonsForTouchEventAt(GUITouchEvent Event, uint16_t XPos, uint16_t YPos)
{
	static GUIButton* lastActiveButton = 0;
	static GUIButtonState lastState = GUIButtonState_NoState;

	for (uint32_t index = 0; index < guiConfigNUMBER_OF_BUTTONS; index++)
	{
		GUIButton* activeButton = &prvButton_list[index];
		/* Check if the button is not hidden and enabled and if it's hit */
		if (activeButton->object.displayState == GUIDisplayState_NotHidden && activeButton->state != GUIButtonState_NoState &&
			activeButton->object.layer == prvCurrentlyActiveLayer && activeButton->state != GUIButtonState_DisabledTouch &&
			XPos >= activeButton->object.xPos && XPos <= activeButton->object.xPos + activeButton->object.width &&
			YPos >= activeButton->object.yPos && YPos <= activeButton->object.yPos + activeButton->object.height)
		{
			if (Event == GUITouchEvent_Up)
			{
				GUI_SetButtonState(index + guiConfigBUTTON_ID_OFFSET, lastState);
				lastActiveButton = 0;
				lastState = GUIButtonState_Disabled;
				if (activeButton->touchCallback != 0)
				{
					activeButton->touchCallback(Event, index + guiConfigBUTTON_ID_OFFSET);
					if (prvBeepIsOn)
						BUZZER_BeepNumOfTimes(1);
				}
			}
			else if (Event == GUITouchEvent_Down)
			{
				/*
				 * Check if the new button we have hit is different from the last time,
				 * if so change back the state of the old button and activate the new one
				 * and save a reference to it
				 */
				if (lastActiveButton == 0 || lastActiveButton->object.id != activeButton->object.id)
				{
					/*
					 * If we had a last active button it means the user has moved away from the button while
					 * still holding down on the screen. We therefore have to reset the state of that button
					 */
					if (lastActiveButton != 0)
						GUI_SetButtonState(lastActiveButton->object.id, lastState);

					/* Save the new button and change it's state */
					lastState = activeButton->state;
					lastActiveButton = &prvButton_list[index];
					GUI_SetButtonState(index + guiConfigBUTTON_ID_OFFSET, GUIButtonState_TouchDown);
				}
				/* Otherwise just call the callback as the user is still touching the same button */
				if (activeButton->touchCallback != 0)
					activeButton->touchCallback(Event, index + guiConfigBUTTON_ID_OFFSET);
			}
			/* Only one button should be active on an event so return when we have found one */
			return;
		}
	}

	/* If no button hit was found, check if a button is still in touch down state and if so change it's state */
	if (lastActiveButton != 0)
	{
		GUI_SetButtonState(lastActiveButton->object.id, lastState);
		lastActiveButton = 0;
		lastState = GUIButtonState_NoState;
	}
}

/**
 * @brief	Get the display state of a button
 * @param	ContainerId: The button to get the state for
 * @retval	The display state if valid ID, otherwise GUIDisplayState_NoState
 */
GUIDisplayState GUI_GetDisplayStateForButton(uint32_t ButtonId)
{
	uint32_t index = ButtonId - guiConfigBUTTON_ID_OFFSET;

	if (index < guiConfigNUMBER_OF_BUTTONS)
		return prvButton_list[index].object.displayState;
	else
		return GUIDisplayState_NoState;
}

/**
 * @brief
 * @param	ContainerId: The button to get the state for
 * @param	Layer: The layer to set
 * @retval	None
 */
void GUI_SetLayerForButton(uint32_t ButtonId, GUILayer Layer)
{
	uint32_t index = ButtonId - guiConfigBUTTON_ID_OFFSET;

	if (index < guiConfigNUMBER_OF_BUTTONS)
	{
		prvButton_list[index].object.layer = Layer;
	}
}

/* Text box ------------------------------------------------------------------*/
/**
 * @brief	Get a pointer to the text box corresponding to the id
 * @param	TextBoxId: The id of the text box to get
 * @retval	Pointer the text box or 0 if no text box was found
 */
GUITextBox* GUI_GetTextBoxFromId(uint32_t TextBoxId)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	if (index < guiConfigNUMBER_OF_TEXT_BOXES)
		return &prvTextBox_list[index];
	else
		return 0;
}

/**
 * @brief	Add a text box to the list
 * @param	TextBox: The text box to add
 * @retval	SUCCESS if everything went OK
 * @retval	ERROR: if something went wrong
 */
ErrorStatus GUITextBox_Add(GUITextBox* TextBox)
{
	uint32_t index = TextBox->object.id - guiConfigTEXT_BOX_ID_OFFSET;
	ErrorStatus status;

	/* Make sure we don't try to create more text boxes than there's room for in the textBox_list */
	if (index < guiConfigNUMBER_OF_TEXT_BOXES)
	{
		/* Copy the text box to the list */
		memcpy(&prvTextBox_list[index], TextBox, sizeof(GUITextBox));
		GUITextBox* newTextBox = &prvTextBox_list[index];

		/* Save effective width and height based on how much padding we got */
		newTextBox->effectiveWidth = newTextBox->object.width - newTextBox->padding.left - newTextBox->padding.right;
		newTextBox->effectiveHeight = newTextBox->object.height - newTextBox->padding.top - newTextBox->padding.bottom;

		/* Save how many characters can be displayed */
		newTextBox->maxCharactersPerRow = newTextBox->effectiveWidth / (guiConfigFONT_WIDTH_UNIT * newTextBox->textSize);
		newTextBox->maxRows = newTextBox->effectiveHeight / (guiConfigFONT_HEIGHT_UNIT * newTextBox->textSize);
		newTextBox->maxNumOfCharacters = newTextBox->maxCharactersPerRow * newTextBox->maxRows;

		/* If there's a static text - Fill in the rest of the data */
		if (newTextBox->staticText != 0)
		{
			newTextBox->staticTextNumOfChar = strlen(newTextBox->staticText);
			newTextBox->staticTextWidth = newTextBox->staticTextNumOfChar * guiConfigFONT_WIDTH_UNIT * newTextBox->textSize;
			newTextBox->staticTextHeight = guiConfigFONT_HEIGHT_UNIT * newTextBox->textSize;

			/* Overwrite the write positions so that the text is centered */
			newTextBox->xWritePos = (newTextBox->object.width - newTextBox->staticTextWidth) / 2;
			newTextBox->yWritePos = (newTextBox->object.height - newTextBox->staticTextHeight) / 2 - 2;
		}
		else
		{
			/* Reset write position */
			GUI_SetWritePosition(newTextBox->object.id, 0, 0);
		}

		/* Allocate memory for text */
		newTextBox->textBuffer = pvPortMalloc(newTextBox->maxNumOfCharacters);

		/* If it's set to not hidden we should draw the text box */
		if (newTextBox->object.displayState == GUIDisplayState_NotHidden)
			status = GUITextBox_Draw(TextBox->object.id);
	}
	else
		status = ERROR;

	/* Set all the data in the TextBox we received as a parameter to 0 so that it can be reused easily */
	memset(TextBox, 0, sizeof(GUITextBox));

	return status;
}

/**
 * @brief	Hides a text box by drawing a black box over it
 * @param	TextBoxId: The id of the text box to hide
 * @retval	None
 */
void GUITextBox_Hide(uint32_t TextBoxId)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	/* Make sure the index is valid and that the correct layer is active */
	if (index < guiConfigNUMBER_OF_TEXT_BOXES && prvTextBox_list[index].object.layer == prvCurrentlyActiveLayer)
	{
		/* Set the background color */
		LCD_SetBackgroundColor(LCD_COLOR_BLACK);
		/* Clear the active window */
		LCDActiveWindow window;
		window.xLeft = prvTextBox_list[index].object.xPos;
		window.xRight = prvTextBox_list[index].object.xPos + prvTextBox_list[index].object.width - 1;
		window.yTop = prvTextBox_list[index].object.yPos;
		window.yBottom = prvTextBox_list[index].object.yPos + prvTextBox_list[index].object.height - 1;
		LCD_ClearActiveWindow(window.xLeft, window.xRight, window.yTop, window.yBottom);
		prvTextBox_list[index].object.displayState = GUIDisplayState_Hidden;
	}
}

/**
 * @brief	Draw a text box
 * @param	TextBoxId: The id of the text box to draw
  * @retval	SUCCESS if everything went OK
 * @retval	ERROR: if something went wrong
 */
ErrorStatus GUITextBox_Draw(uint32_t TextBoxId)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	/* Make sure the index is valid and that the correct layer is active */
	if (index < guiConfigNUMBER_OF_TEXT_BOXES && prvTextBox_list[index].object.layer == prvCurrentlyActiveLayer)
	{
		GUITextBox* textBox = &prvTextBox_list[index];

		/* Set the background color */
		LCD_SetBackgroundColor(textBox->backgroundColor);
		/* Clear the active window */
		LCDActiveWindow window;
		window.xLeft = textBox->object.xPos;
		window.xRight = textBox->object.xPos + textBox->object.width - 1;
		window.yTop = textBox->object.yPos;
		window.yBottom = textBox->object.yPos + textBox->object.height - 1;
		LCD_ClearActiveWindow(window.xLeft, window.xRight, window.yTop, window.yBottom);

		/* Draw the border */
		GUI_DrawBorder(textBox->object);

		textBox->object.displayState = GUIDisplayState_NotHidden;

		/* If there's a static text we should write it */
		if (textBox->staticText != 0)
		{
			GUITextBox_WriteString(TextBoxId, textBox->staticText);
		}
		return SUCCESS;
	}
	else
		return ERROR;
}

/**
 * @brief	Draw all text boxes in the textBox_list
 * @param	None
 * @retval	None
 */
void GUITextBox_DrawAll()
{
	for (uint32_t i = 0; i < guiConfigNUMBER_OF_TEXT_BOXES; i++)
	{
		GUITextBox_Draw(guiConfigTEXT_BOX_ID_OFFSET + i);
	}
}

/**
 * @brief	Write a string in a text box
 * @param	TextBoxId: The id of the text box to write in
 * @param	String: The string to write
 * @retval	SUCCESS if everything went OK
 * @retval	ERROR: if something went wrong
 */
ErrorStatus GUITextBox_WriteString(uint32_t TextBoxId, uint8_t* String)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	/* Make sure the index is valid and that the correct layer is active */
	if (index < guiConfigNUMBER_OF_TEXT_BOXES && prvTextBox_list[index].object.layer == prvCurrentlyActiveLayer)
	{
		GUITextBox* textBox = &prvTextBox_list[index];

		/* Set the text color */
		LCD_SetForegroundColor(textBox->textColor);
		/* Get the active window and then write the text in it */
		LCDActiveWindow window;
		window.xLeft = textBox->object.xPos + textBox->padding.left;
		window.xRight = textBox->object.xPos + textBox->object.width - 1 - textBox->padding.right;
		window.yTop = textBox->object.yPos + textBox->padding.top;
		window.yBottom = textBox->object.yPos + textBox->object.height - 1 - textBox->padding.bottom;

		uint16_t xWritePosTemp = textBox->object.xPos + textBox->xWritePos + textBox->padding.left;
		uint16_t yWritePosTemp = textBox->object.yPos + textBox->yWritePos + textBox->padding.top;

		LCD_WriteStringInActiveWindowAtPosition(String, LCDTransparency_Transparent, textBox->textSize, window,
												&xWritePosTemp, &yWritePosTemp);

		/* Only save the next writeposition if the text box is not static */
		if (textBox->staticText == 0)
		{
			textBox->xWritePos = xWritePosTemp - textBox->object.xPos;
			textBox->yWritePos = yWritePosTemp - textBox->object.yPos;
		}
		return SUCCESS;
	}
	else
		return ERROR;
}

/**
 * @brief	Write a buffer in a text box
 * @param	TextBoxId: The id of the text box to write in
 * @param	pBuffer: The buffer to write
 * @param	Size: Size of the buffer
 * @param	Format: The format to use when writing the buffer, can be any value of GUIWriteFormat
 * @retval	SUCCESS if everything went OK
 * @retval	ERROR: if something went wrong
 * @time	~247 us when Size = 64
 */
ErrorStatus GUITextBox_WriteBuffer(uint32_t TextBoxId, uint8_t* pBuffer, uint32_t Size)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	/* Make sure the index is valid and that the correct layer is active */
	if (index < guiConfigNUMBER_OF_TEXT_BOXES && prvTextBox_list[index].object.layer == prvCurrentlyActiveLayer)
	{
		GUITextBox* textBox = &prvTextBox_list[index];

		/* Set the text color */
		LCD_SetForegroundColor(textBox->textColor);

		/* Get the active window and then write the text in it */
		LCDActiveWindow window;
		window.xLeft = textBox->object.xPos + textBox->padding.left;
		window.xRight = textBox->object.xPos + textBox->object.width - 1 - textBox->padding.right;
		window.yTop = textBox->object.yPos + textBox->padding.top;
		window.yBottom = textBox->object.yPos + textBox->object.height - 1 - textBox->padding.bottom;

		uint16_t xWritePosTemp = textBox->object.xPos + textBox->xWritePos;
		uint16_t yWritePosTemp = textBox->object.yPos + textBox->yWritePos;


		LCD_WriteBufferInActiveWindowAtPosition(pBuffer, Size, LCDTransparency_Transparent, textBox->textSize,
												window,	&xWritePosTemp, &yWritePosTemp);

		/* Only save the next write position if the text box is not static */
		if (textBox->staticText == 0)
		{
			textBox->xWritePos = xWritePosTemp - textBox->object.xPos;
			textBox->yWritePos = yWritePosTemp - textBox->object.yPos;
		}
		return SUCCESS;
	}
	else
		return ERROR;
}

/**
 * @brief
 * @param	TextBoxId: The id of the text box to clear displayed data of
 * @retval	SUCCESS if everything went OK
 * @retval	ERROR: if something went wrong
 * @note	If pFormattedSize = 0 it means no formatting was done and the source data should be used instead
 */
ErrorStatus GUITextBox_FormatDataForTextBox(uint32_t TextBoxId, const uint8_t* pSourceData, const uint32_t SourceSize,
											uint8_t* pFormattedData, uint32_t* pFormattedSize)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	/* Make sure the index is valid */
	if (index < guiConfigNUMBER_OF_TEXT_BOXES)
	{
		GUITextBox* textBox = &prvTextBox_list[index];

		if (textBox->textFormat == GUITextFormat_ASCII)
		{
			*pFormattedSize = 0;
		}
		else if (textBox->textFormat == GUITextFormat_HexWithSpaces)
		{
			for (uint32_t i = 0; i < SourceSize; i++)
			{
				pFormattedData[i*3] = prvHexTable[pSourceData[i] >> 4];
				pFormattedData[i*3+1] = prvHexTable[pSourceData[i] & 0x0F];
				pFormattedData[i*3+2] = ' ';
				*pFormattedSize += 3;
			}
		}
		else if (textBox->textFormat == GUITextFormat_HexWithoutSpaces)
		{
			for (uint32_t i = 0; i < SourceSize; i++)
			{
				pFormattedData[i*2] = prvHexTable[pSourceData[i] >> 4];
				pFormattedData[i*2+1] = prvHexTable[pSourceData[i] & 0x0F];
				*pFormattedSize += 2;
			}
		}

		return SUCCESS;
	}
	else
		return ERROR;
}

/**
 * @brief	Get the number of characters currently displayed on the screen.
 * @param	TextBoxId: The id of the text box to check
 * @retval	The number of characters we are displaying
 * @retval	0: if something went wrong or the count is 0
 */
uint32_t GUITextBox_GetNumOfCharactersDisplayed(uint32_t TextBoxId)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	/* Make sure the index is valid */
	if (index < guiConfigNUMBER_OF_TEXT_BOXES)
	{
		return prvTextBox_list[index].bufferCount;
	}
	else
		return 0;
}

/**
 * @brief	Get the maximum number of characters that can be displayed on the screen in the text box.
 * @param	TextBoxId: The id of the text box to check
 * @retval	The maximum number of characters
 * @retval	0: if something went wrong or the maximum is 0
 */
uint32_t GUITextBox_GetMaxNumOfCharacters(uint32_t TextBoxId)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	/* Make sure the index is valid */
	if (index < guiConfigNUMBER_OF_TEXT_BOXES)
	{
		return prvTextBox_list[index].maxNumOfCharacters;
	}
	else
		return 0;
}

/**
 * @brief	Get the maximum number of characters per row that can be displayed on the screen in the text box.
 * @param	TextBoxId: The id of the text box to check
 * @retval	The maximum number of characters per row
 * @retval	0: if something went wrong or the maximum is 0
 */
uint32_t GUITextBox_GetMaxCharactersPerRow(uint32_t TextBoxId)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	/* Make sure the index is valid */
	if (index < guiConfigNUMBER_OF_TEXT_BOXES)
	{
		return prvTextBox_list[index].maxCharactersPerRow;
	}
	else
		return 0;
}

/**
 * @brief	Get the maximum number of rows that can be displayed on the screen in the text box.
 * @param	TextBoxId: The id of the text box to check
 * @retval	The maximum number of rows
 * @retval	0: if something went wrong or the maximum is 0
 */
uint32_t GUITextBox_GetMaxRows(uint32_t TextBoxId)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	/* Make sure the index is valid */
	if (index < guiConfigNUMBER_OF_TEXT_BOXES)
	{
		return prvTextBox_list[index].maxRows;
	}
	else
		return 0;
}

/**
 * @brief	Set the textformat for the text box
 * @param	TextBoxId: The id of the text box to set
 * @retval	SUCCESS if everything went OK
 * @retval	ERROR: if something went wrong
 */
ErrorStatus GUITextBox_SetTextFormat(uint32_t TextBoxId, GUITextFormat Format)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	/* Make sure the index is valid */
	if (index < guiConfigNUMBER_OF_TEXT_BOXES)
	{
		GUITextBox* textBox = &prvTextBox_list[index];

		textBox->textFormat = Format;

		return SUCCESS;
	}
	else
		return ERROR;
}

/**
 * @brief	Write a number in a text box
 * @param	TextBoxId: The id of the text box to write in
 * @param	Number: The number to write
 * @retval	SUCCESS if everything went OK
 * @retval	ERROR: if something went wrong
 */
ErrorStatus GUITextBox_WriteNumber(uint32_t TextBoxId, int32_t Number)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	if (index < guiConfigNUMBER_OF_TEXT_BOXES)
	{
		uint8_t buffer[20];
		prvItoa(Number, buffer);
		return GUITextBox_WriteString(TextBoxId, buffer);
	}
	else
		return ERROR;
}

/**
 * @brief	Set the static text of the text box
 * @param	TextBoxId: The id of the text box to set
 * @param	String: The text to set
 * @retval	SUCCESS if everything went OK
 * @retval	ERROR: if something went wrong
 */
ErrorStatus GUITextBox_SetStaticText(uint32_t TextBoxId, uint8_t* String)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	if (index < guiConfigNUMBER_OF_TEXT_BOXES)
	{
		GUITextBox* textBox = &prvTextBox_list[index];
		/* Save the new string */
		textBox->staticText = String;

		/* Update the size variables */
		textBox->staticTextNumOfChar = strlen(textBox->staticText);
		textBox->staticTextWidth = textBox->staticTextNumOfChar * guiConfigFONT_WIDTH_UNIT * textBox->textSize;
		textBox->staticTextHeight = guiConfigFONT_HEIGHT_UNIT * textBox->textSize;

		/* Overwrite the write positions so that the text is centered */
		textBox->xWritePos = (textBox->object.width - textBox->staticTextWidth) / 2;
		textBox->yWritePos = (textBox->object.height - textBox->staticTextHeight) / 2 - 2;

		/* Draw the text box with the new static text */
		GUITextBox_Draw(TextBoxId);

		return SUCCESS;
	}
	else
		return ERROR;
}

/**
 * @brief	Go to a newline in textbox
 * @param	TextBoxId:
 * @retval	None
 */
void GUITextBox_NewLine(uint32_t TextBoxId)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	if (index < guiConfigNUMBER_OF_TEXT_BOXES)
	{
		prvTextBox_list[index].xWritePos = 0;
		prvTextBox_list[index].yWritePos += prvTextBox_list[index].textSize * guiConfigFONT_HEIGHT_UNIT;
	}
}

/**
 * @brief	Append data to the displayed text by reading from memory to the address specified by NewEndAddress
 * @param	TextBoxId: The id of the text box
 * @param	NewEndAddress: The new end address where where data should be read to
 * @retval	SUCCESS if everything went OK
 * @retval	ERROR: if something went wrong
 */
ErrorStatus GUITextBox_AppendDataFromMemory(uint32_t TextBoxId, uint32_t NewEndAddress)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	if (index < guiConfigNUMBER_OF_TEXT_BOXES)
	{
		GUITextBox* textBox = &prvTextBox_list[index];

		if (textBox->dataReadFunction != 0)
		{
			/* Get the data from memory */
			uint32_t numOfNewBytes = NewEndAddress - textBox->readEndAddress;

			/* Check if the new data will cause it to append beyond the buffer capability */
			if (textBox->bufferCount + numOfNewBytes > textBox->maxNumOfCharacters)
			{
				uint32_t numOfRowsToMove = numOfNewBytes / textBox->maxCharactersPerRow + 1;
				/* Increment the start address by an amount of rows */
				textBox->readStartAddress += numOfRowsToMove * textBox->maxCharactersPerRow;
				/* Refresh the text box now that we have changed the start address */
				GUITextBox_RefreshCurrentDataFromMemory(TextBoxId);
			}

			/* Add the new data from memory and update the end address */
			textBox->dataReadFunction(&textBox->textBuffer[textBox->bufferCount], textBox->readEndAddress, numOfNewBytes);
			textBox->readEndAddress = NewEndAddress;
			textBox->readLastValidByteAddress = NewEndAddress;

			/* Set the text color */
			LCD_SetForegroundColor(textBox->textColor);

			/* Get the active window and then write the text in it */
			LCDActiveWindow window;
			window.xLeft = textBox->object.xPos + textBox->padding.left;
			window.xRight = textBox->object.xPos + textBox->object.width - 1 - textBox->padding.right;
			window.yTop = textBox->object.yPos + textBox->padding.top;
			window.yBottom = textBox->object.yPos + textBox->object.height - 1 - textBox->padding.bottom;

			uint16_t xWritePosTemp = textBox->object.xPos + textBox->xWritePos;
			uint16_t yWritePosTemp = textBox->object.yPos + textBox->yWritePos;

			LCD_WriteBufferInActiveWindowAtPosition(&textBox->textBuffer[textBox->bufferCount], numOfNewBytes,
									LCDTransparency_Transparent, textBox->textSize, window, &xWritePosTemp, &yWritePosTemp);

			/* Update the write positions */
			textBox->xWritePos = xWritePosTemp - textBox->object.xPos;
			textBox->yWritePos = yWritePosTemp - textBox->object.yPos;

			/* Update the buffer count to reflect the new amount of data it holds */
			textBox->bufferCount += numOfNewBytes;

			return SUCCESS;
		}
	}

error:
	return ERROR;
}

/**
 * @brief	Refresh the displayed data by clearing the textbox and reading the data from memory again
 * @param	TextBoxId: The id of the text box
 * @retval	SUCCESS if everything went OK
 * @retval	ERROR: if something went wrong
 */
ErrorStatus GUITextBox_RefreshCurrentDataFromMemory(uint32_t TextBoxId)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	if (index < guiConfigNUMBER_OF_TEXT_BOXES)
	{
		GUITextBox* textBox = &prvTextBox_list[index];

		if (textBox->dataReadFunction != 0)
		{
			/* Calculate how many bytes we should read */
			uint32_t numOfBytesToRead = textBox->readEndAddress - textBox->readStartAddress;
			/* Update the buffer count to reflect the new amount of data it holds */
			textBox->bufferCount = numOfBytesToRead;
			/* Get the data from memory */
			textBox->dataReadFunction(textBox->textBuffer, textBox->readStartAddress, numOfBytesToRead);

			ErrorStatus status;
			/* Redraw the text box */
			status = GUITextBox_Draw(TextBoxId);
			if (status != SUCCESS)
				goto error;

			/* Set the write position to the upper left corner */
			GUI_SetWritePosition(TextBoxId, 0, 0);

			/* Set the text color */
			LCD_SetForegroundColor(textBox->textColor);

			/* Get the active window and then write the text in it */
			LCDActiveWindow window;
			window.xLeft = textBox->object.xPos + textBox->padding.left;
			window.xRight = textBox->object.xPos + textBox->object.width - 1 - textBox->padding.right;
			window.yTop = textBox->object.yPos + textBox->padding.top;
			window.yBottom = textBox->object.yPos + textBox->object.height - 1 - textBox->padding.bottom;

			uint16_t xWritePosTemp = textBox->object.xPos + textBox->xWritePos;
			uint16_t yWritePosTemp = textBox->object.yPos + textBox->yWritePos;

			LCD_WriteBufferInActiveWindowAtPosition(textBox->textBuffer, textBox->bufferCount,
									LCDTransparency_Transparent, textBox->textSize, window, &xWritePosTemp, &yWritePosTemp);

			/* Update the write positions */
			textBox->xWritePos = xWritePosTemp - textBox->object.xPos;
			textBox->yWritePos = yWritePosTemp - textBox->object.yPos;

			return SUCCESS;
		}
	}

error:
	return ERROR;
}

/**
 * @brief	Move the currently displayed data an amount of rows. This can be seen as moving a virtual window inside
 * 			the memory where the data is displayed.
 * @param	TextBoxId: The id of the text box
 * @param	NumOfRows: Number of rows to move
 * @retval	SUCCESS if everything went OK
 * @retval	ERROR: if something went wrong
 */
ErrorStatus GUITextBox_MoveDisplayedDataNumOfRows(uint32_t TextBoxId, int32_t NumOfRows)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	if (index < guiConfigNUMBER_OF_TEXT_BOXES)
	{
		GUITextBox* textBox = &prvTextBox_list[index];

		/* Only allow movement if we have saved more than one page of data */
		if (textBox->readLastValidByteAddress - textBox->readMinAddress > textBox->maxNumOfCharacters && NumOfRows != 0)
		{
			/* Move forward numOfRows in memory */
			if (NumOfRows < 0 && textBox->readEndAddress != textBox->readLastValidByteAddress)
			{
				/* Indicate that we are scrolling */
				textBox->isScrolling = true;

				/* Get the number of bytes to move */
				int32_t numOfBytesToMove = NumOfRows * textBox->maxCharactersPerRow;

				/* Move the end address forward in memory a certain amount of rows */
				textBox->readEndAddress -= numOfBytesToMove;

				/* Check if we went to far */
				uint32_t numOfDataOnLastRow = 0;
				if (textBox->readEndAddress > textBox->readLastValidByteAddress)
				{
					textBox->readEndAddress = textBox->readLastValidByteAddress;
					/* If we have reached the last valid byte it means we have stopped scrolling */
					textBox->isScrolling = false;
					/* Get how many characters there are on the last row */
					numOfDataOnLastRow = textBox->readEndAddress % textBox->maxCharactersPerRow;
				}

				/* Check if the last row was not filled and take that into consideration */
				if (numOfDataOnLastRow)
					textBox->readStartAddress = textBox->readEndAddress - numOfDataOnLastRow - (textBox->maxRows-1)*textBox->maxCharactersPerRow;
				else
					textBox->readStartAddress = textBox->readEndAddress - textBox->maxRows*(textBox->maxCharactersPerRow);
			}
			/* Move backward numOfRows in memory */
			else if (NumOfRows > 0 && textBox->readStartAddress != textBox->readMinAddress)
			{
				/* Indicate that we are scrolling */
				textBox->isScrolling = true;

				/* Get the number of bytes to move */
				int32_t numOfBytesToMove = NumOfRows * textBox->maxCharactersPerRow;
				/* Move back in memory by a certain amount of rows */
				textBox->readStartAddress -= numOfBytesToMove;

				/* Check if we went to far back */
				if (textBox->readStartAddress < textBox->readMinAddress)
					textBox->readStartAddress = textBox->readMinAddress;

				/* Set the end address so that we will fill the entire text box with text */
				textBox->readEndAddress = textBox->readStartAddress + textBox->maxNumOfCharacters;
			}
			else
			{
				goto error;
			}

			/* Refresh the displayed data now that we have changed the limits */
			GUITextBox_RefreshCurrentDataFromMemory(TextBoxId);

			return SUCCESS;
		}
	}
error:
	return ERROR;
}

/**
 * @brief	Clear the displayed data for a text box by resetting the buffer and redrawing the text box
 * @param	TextBoxId: The id of the text box to clear displayed data of
 * @retval	SUCCESS if everything went OK
 * @retval	ERROR: if something went wrong
 */
ErrorStatus GUITextBox_ClearDisplayedData(uint32_t TextBoxId)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	/* Make sure the index is valid */
	if (index < guiConfigNUMBER_OF_TEXT_BOXES)
	{
		GUITextBox* textBox = &prvTextBox_list[index];

		textBox->bufferCount = 0;

		/* Reset write position */
		GUI_SetWritePosition(TextBoxId, 0, 0);

		/* Redraw the text box */
		GUITextBox_Draw(TextBoxId);

		return SUCCESS;
	}
	else
		return ERROR;
}

/**
 * @brief	Get the current read end address of the displayed data
 * @param	TextBoxId: The id of the text box
 * @retval	The end address or 0 if something went wrong
 */
uint32_t GUITextBox_GetReadEndAddress(uint32_t TextBoxId)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	/* Make sure the index is valid */
	if (index < guiConfigNUMBER_OF_TEXT_BOXES)
	{
		return prvTextBox_list[index].readEndAddress;
	}
	else
		return 0;
}

/**
 * @brief	Set the read end, start and last valid address. This can be used when clearing.
 * @param	TextBoxId: The id of the text box
 * @param	NewAddress: The new address
 * @retval	The end address or 0 if something went wrong
 */
void GUITextBox_SetAddressesTo(uint32_t TextBoxId, uint32_t NewAddress)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	/* Make sure the index is valid */
	if (index < guiConfigNUMBER_OF_TEXT_BOXES)
	{
		prvTextBox_list[index].readEndAddress = NewAddress;
		prvTextBox_list[index].readStartAddress = NewAddress;
		prvTextBox_list[index].readLastValidByteAddress = NewAddress;
	}
}

/**
 * @brief	Check if the text box is in scrolling mode or not
 * @param	TextBoxId: The id of the text box
 * @retval	True if it's scrolling otherwise false, if something is wrong it will give false as well
 */
bool GUITextBox_IsScrolling(uint32_t TextBoxId)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	/* Make sure the index is valid */
	if (index < guiConfigNUMBER_OF_TEXT_BOXES)
	{
		return prvTextBox_list[index].isScrolling;
	}
	else
		return false;
}


/**
 * @brief	Set where the next character should be written
 * @param	TextBoxId:
 * @param	XPos:
 * @param	YPos:
 * @retval	None
 */
void GUI_SetWritePosition(uint32_t TextBoxId, uint16_t XPos, uint16_t YPos)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	if (index < guiConfigNUMBER_OF_TEXT_BOXES)
	{
		prvTextBox_list[index].xWritePos = XPos + prvTextBox_list[index].padding.left;
		prvTextBox_list[index].yWritePos = YPos + prvTextBox_list[index].padding.top;
	}
}

/**
 * @brief	Set the y-write position to the center of the textbox
 * @param	TextBoxId:
 * @retval	None
 */
void GUI_SetYWritePositionToCenter(uint32_t TextBoxId)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	if (index < guiConfigNUMBER_OF_TEXT_BOXES)
	{
		prvTextBox_list[index].yWritePos =
				(prvTextBox_list[index].object.height - guiConfigFONT_HEIGHT_UNIT * prvTextBox_list[index].textSize) / 2 - 2;
	}
}

/**
 * @brief	Get where the next character should be written
 * @param	TextBoxId:
 * @param	XPos:
 * @param	YPos:
 * @retval	None
 */
void GUI_GetWritePosition(uint32_t TextBoxId, uint16_t* XPos, uint16_t* YPos)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	if (index < guiConfigNUMBER_OF_TEXT_BOXES)
	{
		*XPos = prvTextBox_list[index].xWritePos;
		*YPos = prvTextBox_list[index].yWritePos;
	}
}

/**
 * @brief	Clear the text box of any text
 * @param	TextBoxId:
 * @retval	SUCCESS if everything went OK
 * @retval	ERROR: if something went wrong
 */
ErrorStatus GUI_ClearTextBox(uint32_t TextBoxId)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	if (index < guiConfigNUMBER_OF_TEXT_BOXES && prvTextBox_list[index].object.layer == prvCurrentlyActiveLayer)
	{
		return GUITextBox_Draw(TextBoxId);
	}
	else
		return ERROR;
}

/**
 * @brief	Clear the text box of any text and reset the write position to 0,0
 * @param	TextBoxId:
 * @retval	SUCCESS if everything went OK
 * @retval	ERROR: if something went wrong
 */
ErrorStatus GUI_ClearAndResetTextBox(uint32_t TextBoxId)
{
	GUI_SetWritePosition(TextBoxId, 0, 0);
	return GUI_ClearTextBox(TextBoxId);
}



/**
 * @brief	Check if a text box is located at the position where a touch up event occurred
 * @param	GUITouchEvent: The event that happened, can be any value of GUITouchEvent
 * @param	XPos: X-position for event
 * @param	XPos: Y-position for event
 * @retval	None
 */
void GUI_CheckAllActiveTextBoxesForTouchEventAt(GUITouchEvent Event, uint16_t XPos, uint16_t YPos)
{
	for (uint32_t index = 0; index < guiConfigNUMBER_OF_TEXT_BOXES; index++)
	{
		GUITextBox* activeTextBox = &prvTextBox_list[index];
		/* Check if the button is not hidden and enabled and if it's hit */
		if (activeTextBox->object.displayState == GUIDisplayState_NotHidden &&
			activeTextBox->object.layer == prvCurrentlyActiveLayer &&
			XPos >= activeTextBox->object.xPos && XPos <= activeTextBox->object.xPos + activeTextBox->object.width &&
			YPos >= activeTextBox->object.yPos && YPos <= activeTextBox->object.yPos + activeTextBox->object.height)
		{
			if (activeTextBox->touchCallback != 0)
				activeTextBox->touchCallback(Event, XPos, YPos);
			/* Only one text box should be active on an event so return when we have found one */
			return;
		}
	}
}

/**
 * @brief	Get the display state of a text box
 * @param	TextBoxId: The text box to get the state for
 * @retval	The display state if valid ID, otherwise GUIDisplayState_NoState
 */
GUIDisplayState GUI_GetDisplayStateForTextBox(uint32_t TextBoxId)
{
	uint32_t index = TextBoxId - guiConfigTEXT_BOX_ID_OFFSET;

	if (index < guiConfigNUMBER_OF_TEXT_BOXES)
		return prvTextBox_list[index].object.displayState;
	else
		return GUIDisplayState_NoState;
}

/* Container -----------------------------------------------------------------*/
/**
 * @brief	Get a pointer to the container corresponding to the id
 * @param	ContainerId: The id of the container to get
 * @retval	Pointer the container or 0 if no container was found
 */
GUIContainer* GUI_GetContainerFromId(uint32_t ContainerId)
{
	uint32_t index = ContainerId - guiConfigCONTAINER_ID_OFFSET;

	if (index < guiConfigNUMBER_OF_CONTAINERS)
		return &prvContainer_list[index];
	else
		return 0;
}

/**
 * @brief	Add a container to the list
 * @param	Container: Pointer to the container to add
 * @retval	SUCCESS if everything went OK
 * @retval	ERROR: if something went wrong
 */
ErrorStatus GUI_AddContainer(GUIContainer* Container)
{
	uint32_t index = Container->object.id - guiConfigCONTAINER_ID_OFFSET;
	ErrorStatus status;

	/* Make sure we don't try to create more containers than there's room for in the textBox_list */
	if (index < guiConfigNUMBER_OF_CONTAINERS)
	{
		/* Copy the container to the list */
		memcpy(&prvContainer_list[index], Container, sizeof(GUIContainer));

		/* If it's set to not hidden we should draw the button */
		if (Container->object.displayState == GUIDisplayState_NotHidden)
			status = GUI_DrawContainer(Container->object.id);
	}
	else
		status = ERROR;

	/* Set all the data in the Container we received as a parameter to 0 so that it can be reused easily */
	memset(Container, 0, sizeof(GUIContainer));

	return status;
}

/**
 * @brief	Hide the content in a container
 * @param	ContainerId: The id of the container to hide content of
 * @retval	None
 */
void GUI_HideContentInContainer(uint32_t ContainerId)
{
	uint32_t index = ContainerId - guiConfigCONTAINER_ID_OFFSET;

	if (index < guiConfigNUMBER_OF_CONTAINERS)
	{
		/* Hide the buttons */
		for (uint32_t i = 0; i < guiConfigNUMBER_OF_BUTTONS; i++)
		{
			if (prvContainer_list[index].buttons[i] != 0 &&
				prvContainer_list[index].buttons[i]->object.displayState == GUIDisplayState_NotHidden)
				GUI_HideButton(prvContainer_list[index].buttons[i]->object.id);
		}

		/* Hide the text boxes */
		for (uint32_t i = 0; i < guiConfigNUMBER_OF_TEXT_BOXES; i++)
		{
			if (prvContainer_list[index].textBoxes[i] != 0 &&
				prvContainer_list[index].textBoxes[i]->object.displayState == GUIDisplayState_NotHidden)
				GUITextBox_Hide(prvContainer_list[index].textBoxes[i]->object.id);
		}

		/* Hide the containers */
		for (uint32_t i = 0; i < guiConfigNUMBER_OF_CONTAINERS; i++)
		{
			if (prvContainer_list[index].containers[i] != 0 &&
				prvContainer_list[index].containers[i]->object.displayState == GUIDisplayState_NotHidden)
				GUI_HideContainer(prvContainer_list[index].containers[i]->object.id);
		}


		/* Check if borders should be drawn */
		if (prvContainer_list[index].contentHideState == GUIHideState_KeepBorders)
			GUI_DrawBorder(prvContainer_list[index].object);

		prvContainer_list[index].object.displayState = GUIDisplayState_ContentHidden;
	}
}

/**
 * @brief	Hide a cointainer
 * @param	ContainerId: The id of the container to hide
 * @retval	None
 */
void GUI_HideContainer(uint32_t ContainerId)
{
	uint32_t index = ContainerId - guiConfigCONTAINER_ID_OFFSET;

	if (index < guiConfigNUMBER_OF_CONTAINERS)
	{
		/* Set the background color */
		LCD_SetBackgroundColor(LCD_COLOR_BLACK);
		/* Clear the active window */
		LCDActiveWindow window;
		window.xLeft = prvContainer_list[index].object.xPos;
		window.xRight = prvContainer_list[index].object.xPos + prvContainer_list[index].object.width - 1;
		window.yTop = prvContainer_list[index].object.yPos;
		window.yBottom = prvContainer_list[index].object.yPos + prvContainer_list[index].object.height - 1;
		LCD_ClearActiveWindow(window.xLeft, window.xRight, window.yTop, window.yBottom);

		/* Hide the buttons */
		for (uint32_t i = 0; i < guiConfigNUMBER_OF_BUTTONS; i++)
		{
			if (prvContainer_list[index].buttons[i] != 0)
				GUI_HideButton(prvContainer_list[index].buttons[i]->object.id);
		}

		/* Hide the text boxes */
		for (uint32_t i = 0; i < guiConfigNUMBER_OF_TEXT_BOXES; i++)
		{
			if (prvContainer_list[index].textBoxes[i] != 0)
				GUITextBox_Hide(prvContainer_list[index].textBoxes[i]->object.id);
		}

		/* Hide the containers */
		for (uint32_t i = 0; i < guiConfigNUMBER_OF_CONTAINERS; i++)
		{
			if (prvContainer_list[index].containers[i] != 0)
				GUI_HideContainer(prvContainer_list[index].containers[i]->object.id);
		}

		prvContainer_list[index].object.displayState = GUIDisplayState_Hidden;
	}
}

/**
 * @brief	Draw a specific container with the specified id
 * @param	ContainerId: The id of the container to draw
 * @retval	SUCCESS if everything went OK
 * @retval	ERROR: if something went wrong
 */
ErrorStatus GUI_DrawContainer(uint32_t ContainerId)
{
	uint32_t index = ContainerId - guiConfigCONTAINER_ID_OFFSET;

	/* Make sure the index is valid and that the correct layer is active */
	if (index < guiConfigNUMBER_OF_CONTAINERS && prvContainer_list[index].object.layer == prvCurrentlyActiveLayer)
	{
		GUIContainer* container = &prvContainer_list[index];

		/* Set the background color */
		LCD_SetBackgroundColor(container->backgroundColor);
		/* Clear the active window */
		LCDActiveWindow window;
		window.xLeft = prvContainer_list[index].object.xPos;
		window.xRight = prvContainer_list[index].object.xPos + prvContainer_list[index].object.width - 1;
		window.yTop = prvContainer_list[index].object.yPos;
		window.yBottom = prvContainer_list[index].object.yPos + prvContainer_list[index].object.height - 1;
		LCD_ClearActiveWindow(window.xLeft, window.xRight, window.yTop, window.yBottom);

		/* Draw the buttons */
		for (uint32_t i = 0; i < guiConfigNUMBER_OF_BUTTONS; i++)
		{
			if (container->buttons[i] != 0 && ((container->buttons[i]->object.containerPage & container->activePage) ||
					(container->buttons[i]->object.containerPage == container->activePage)))
				GUI_DrawButton(container->buttons[i]->object.id);
		}

		/* Draw the text boxes */
		for (uint32_t i = 0; i < guiConfigNUMBER_OF_TEXT_BOXES; i++)
		{
			if (container->textBoxes[i] != 0 && ((container->textBoxes[i]->object.containerPage & container->activePage) ||
					(container->textBoxes[i]->object.containerPage == container->activePage)))
				GUITextBox_Draw(container->textBoxes[i]->object.id);
		}

		/* Draw the containers */
		for (uint32_t i = 0; i < guiConfigNUMBER_OF_CONTAINERS; i++)
		{
			if (container->containers[i] != 0 && ((container->containers[i]->object.containerPage & container->activePage) ||
					(container->containers[i]->object.containerPage == container->activePage)))
				GUI_DrawContainer(container->containers[i]->object.id);
		}

		/* Draw the border */
		GUI_DrawBorder(container->object);

		container->object.displayState = GUIDisplayState_NotHidden;

		return SUCCESS;
	}
	else
		return ERROR;
}

/**
 * @brief	Change the page of the container
 * @param	ContainerId: The id of the container to change page on
 * @param	NewPage: The new page to use, can be any value of GUIContainerPage
 * @retval	None
 */
void GUI_ChangePageOfContainer(uint32_t ContainerId, GUIContainerPage NewPage)
{
	uint32_t index = ContainerId - guiConfigCONTAINER_ID_OFFSET;

	/* Make sure the index is valid */
	if (index < guiConfigNUMBER_OF_CONTAINERS)
	{
		if (prvContainer_list[index].activePage != NewPage)
		{
			prvContainer_list[index].activePage = NewPage;
			GUI_HideContentInContainer(ContainerId);
			GUI_DrawContainer(ContainerId);
		}
	}
}

/**
 * @brief	Get the currently active page of the container
 * @param	ContainerId: The id of the container to check
 * @retval	None
 */
GUIContainerPage GUI_GetActivePageOfContainer(uint32_t ContainerId)
{
	uint32_t index = ContainerId - guiConfigCONTAINER_ID_OFFSET;

	/* Make sure the index is valid */
	if (index < guiConfigNUMBER_OF_CONTAINERS)
	{
		return prvContainer_list[index].activePage;
	}

	/* Something is wrong with the ID */
	return GUIContainerPage_None;
}

/**
 * @brief	Get the last page of the container
 * @param	ContainerId: The id of the container to check
 * @retval	None
 */
GUIContainerPage GUI_GetLastPageOfContainer(uint32_t ContainerId)
{
	uint32_t index = ContainerId - guiConfigCONTAINER_ID_OFFSET;

	/* Make sure the index is valid */
	if (index < guiConfigNUMBER_OF_CONTAINERS)
	{
		return prvContainer_list[index].lastPage;
	}

	/* Something is wrong with the ID */
	return GUIContainerPage_None;
}

/**
 * @brief	Increase the page of the container by one
 * @param	ContainerId: The id of the container to check
 * @retval	None
 */
void GUI_IncreasePageOfContainer(uint32_t ContainerId)
{
	uint32_t index = ContainerId - guiConfigCONTAINER_ID_OFFSET;

	/* Make sure the index is valid */
	if (index < guiConfigNUMBER_OF_CONTAINERS)
	{
		GUIContainer* container = &prvContainer_list[index];
		if (container->activePage != container->lastPage)
		{
			/* Increase the page by one step */
			container->activePage = container->activePage << 1;
			GUI_HideContentInContainer(ContainerId);
			GUI_DrawContainer(ContainerId);
		}
	}
}

/**
 * @brief	Decrease the page of the container by one
 * @param	ContainerId: The id of the container to check
 * @retval	None
 */
void GUI_DecreasePageOfContainer(uint32_t ContainerId)
{
	uint32_t index = ContainerId - guiConfigCONTAINER_ID_OFFSET;

	/* Make sure the index is valid */
	if (index < guiConfigNUMBER_OF_CONTAINERS)
	{
		GUIContainer* container = &prvContainer_list[index];
		if (container->activePage != GUIContainerPage_1 && container->activePage != GUIContainerPage_None)
		{
			/* Decrease the page by one step */
			container->activePage = container->activePage >> 1;
			GUI_HideContentInContainer(ContainerId);
			GUI_DrawContainer(ContainerId);
		}
	}
}

/**
 * @brief	Check if a container is located at the position where a touch up event occurred
 * @param	GUITouchEvent: The event that happened, can be any value of GUITouchEvent
 * @param	XPos: X-position for event
 * @param	XPos: Y-position for event
 * @retval	None
 */
void GUI_CheckAllContainersForTouchEventAt(GUITouchEvent Event, uint16_t XPos, uint16_t YPos)
{
	for (uint32_t index = 0; index < guiConfigNUMBER_OF_CONTAINERS; index++)
	{
		GUIContainer* activeContainer = &prvContainer_list[index];
		/* Check if the container is not hidden and enabled and if it's hit */
		if (activeContainer->object.displayState == GUIDisplayState_NotHidden &&
			activeContainer->object.layer == prvCurrentlyActiveLayer &&
			XPos >= activeContainer->object.xPos && XPos <= activeContainer->object.xPos + activeContainer->object.width &&
			YPos >= activeContainer->object.yPos && YPos <= activeContainer->object.yPos + activeContainer->object.height)
		{
			if (activeContainer->touchCallback != 0)
				activeContainer->touchCallback(Event, XPos, YPos);
			/* Only one container should be active on an event so return when we have found one */
			return;
		}
	}
}

/**
 * @brief	Get the display state of a container
 * @param	ContainerId: The container to get the state for
 * @retval	The display state if valid ID, otherwise GUIDisplayState_NoState
 */
GUIDisplayState GUI_GetDisplayStateForContainer(uint32_t ContainerId)
{
	uint32_t index = ContainerId - guiConfigCONTAINER_ID_OFFSET;

	if (index < guiConfigNUMBER_OF_CONTAINERS)
		return prvContainer_list[index].object.displayState;
	else
		return GUIDisplayState_NoState;
}

/* Private functions ---------------------------------------------------------*/
/**
 * @brief	Convert an int32_t to a c-string
 * @param	Number:
 * @param	Buffer:
 * @retval	None
 * @note	Code from https://code.google.com/p/my-itoa/
 */
static int32_t prvItoa(int32_t Number, uint8_t* Buffer)
{
    const uint32_t radix = 10;

    uint8_t* p;
    uint32_t a;		/* every digit */
    int32_t len;
    uint8_t* b;		/* start of the digit uint8_t */
    uint8_t temp;
    uint32_t u;

    p = Buffer;

    if (Number < 0)
    {
        *p++ = '-';
        Number = 0 - Number;
    }
    u = (uint32_t)Number;

    b = p;

    do
    {
        a = u % radix;
        u /= radix;

        *p++ = a + '0';

    } while (u > 0);

    len = (int32_t)(p - Buffer);

    *p-- = 0;

    /* swap */
    do
    {
        temp = *p;
        *p = *b;
        *b = temp;
        --p;
        ++b;

    } while (b < p);

    return len;
}
/* Interrupt Handlers --------------------------------------------------------*/
