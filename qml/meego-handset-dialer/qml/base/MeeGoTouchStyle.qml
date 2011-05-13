/*
 * Application Styling Component
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
    property color background: '#efefef'
    property color foreground: '#5f5f5f'
    property color foregroundLight: '#bfbfbf'

    property color buttonBorder: '#4f4f4f'
    property color buttonBackground: '#dfdfdf'
    property color buttonText: '#5f5f5f'
    property color buttonDetailText: '#8f8f8f'

    property color windowTitleBackground: '#9f9f9f'
    property color windowToolsNormal: '#bfbfbf'
    property color windowToolsActive: '#dfdfdf'

    function icon(key)
    {
        return '/usr/share/themes/base/meegotouch/icons/' + key + '.svg'
    }
}
