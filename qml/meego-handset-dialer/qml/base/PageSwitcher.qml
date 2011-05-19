/*
 * Application Page Switcher Component
 *
 * Copyright (c) 2011, Tom Swindell.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

import Qt 4.7

Item
{
    id: root

    property alias pages: container.children

    property Item currentPage

    function jumpTo(idx)
    {
        for(var i = 0; i < pages.length; i++)
        {
            pages[i].pos.x = parent.width;
        }
        pages[idx].pos.x = 0;
        currentPage = pages[idx];
    }

    function switchTo(idx)
    {
        if(currentPage == pages[idx]) return;

        for(var i = 0; i < pages.length; i++)
        {
            var page = pages[i];
            if(page != currentPage) page.pos.x = parent.width
        }

        pages[idx].x = 0
        currentPage.x = -parent.width
        currentPage = pages[idx]
    }

    Rectangle
    {
        anchors.fill: parent;
        color: style.background;
    }

    Item
    {
        id: container
        anchors.fill: parent
    }
}
