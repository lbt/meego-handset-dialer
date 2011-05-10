/*
 * Generic Javascript Utility Functions
 *
 * Copyright (c) 2011, Tom Swindell.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

Date.prototype.getFormat = function()
{
    return (this.getDate() < 10 ? '0' : '') + this.getDate() + '/' +
           (this.getMonth() < 10 ? '0' : '') + this.getMonth() + '/' +
           this.getFullYear() +
           ' | ' +
           (this.getHours() < 10 ? '0' : '') + this.getHours() + ':' +
           (this.getMinutes() < 10 ? '0' : '') + this.getMinutes() + ':' +
           (this.getSeconds() < 10 ? '0' : '') + this.getSeconds();
}

function friendlyDuration(duration)
{
    var minutes = Math.floor(duration / 60);
    var seconds = duration % 60;

    return (minutes < 10 ? '0' : '') + minutes + ":" + (seconds < 10 ? '0' : '') + seconds;
}
