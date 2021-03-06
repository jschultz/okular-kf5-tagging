/*
 *   Copyright 2015 by Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; either version 2,
 *   or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.2
import QtQuick.Controls 1.2 as QtControls
import org.kde.okular 2.0
import "./private"

/**
 * A touchscreen optimized view for a document
 * 
 * It supports changing pages by a swipe gesture, pinch zoom
 * and flicking to scroll around
 */
QtControls.ScrollView {
    id: root
    property DocumentItem document
    property PageItem page: mouseArea.currPageDelegate.pageItem
    signal clicked

    onWidthChanged: resizeTimer.restart()
    onHeightChanged: resizeTimer.restart()
    Flickable {
        id: flick
        anchors.fill: parent
        
        clip: true

        Component.onCompleted: {
            flick.contentWidth = flick.width
            flick.contentHeight = flick.width / mouseArea.currPageDelegate.pageRatio
        }
        Connections {
            target: root.document
            onPathChanged: resizeTimer.restart()
        }
        Timer {
            id: resizeTimer
            interval: 250
            onTriggered: {
                flick.contentWidth = flick.width
                flick.contentHeight = flick.width / mouseArea.currPageDelegate.pageRatio
            }
        }

        PinchArea {
            width: flick.contentWidth
            height: flick.contentHeight

            property real initialWidth
            property real initialHeight

            onPinchStarted: {
                initialWidth = mouseArea.currPageDelegate.implicitWidth * mouseArea.currPageDelegate.scaleFactor
                initialHeight = mouseArea.currPageDelegate.implicitHeight * mouseArea.currPageDelegate.scaleFactor
            }

            onPinchUpdated: {
                // adjust content pos due to drag
                flick.contentX += pinch.previousCenter.x - pinch.center.x
                flick.contentY += pinch.previousCenter.y - pinch.center.y

                // resize content
                //use the scale property during pinch, for speed reasons
                if (initialHeight * pinch.scale > flick.height &&
                    initialHeight * pinch.scale < flick.height * 3) {
                    mouseArea.scale = pinch.scale;
                }
                flick.returnToBounds();
            }
            onPinchFinished: {
                flick.resizeContent(Math.max(flick.width+1, initialWidth * mouseArea.scale), Math.max(flick.height, initialHeight * mouseArea.scale), pinch.center);
                mouseArea.scale = 1;

                flick.returnToBounds();
            }
            MouseArea {
                id: mouseArea
                width: parent.width
                height: parent.height

                property real oldMouseX
                property real oldMouseY
                property bool incrementing: true
                property Item currPageDelegate: page1
                property Item prevPageDelegate: page2
                property Item nextPageDelegate: page3

                onPressed: {
                    var pos = mapToItem(flick, mouse.x, mouse.y);
                    oldMouseX = pos.x;
                    oldMouseY = pos.y;
                }
                onPositionChanged: {
                    var pos = mapToItem(flick, mouse.x, mouse.y);
                    currPageDelegate.x += pos.x - oldMouseX;
                    mouseArea.incrementing = currPageDelegate.x <= 0;

                    preventStealing = (currPageDelegate.x > 0 && flick.atXBeginning) || (currPageDelegate.x < 0 && flick.atXEnd);

                    flick.contentY = Math.max(0, Math.min(flick.contentHeight - flick.height, flick.contentY - (pos.y - oldMouseY)));

                    oldMouseX = pos.x;
                    oldMouseY = pos.y;
                }
                onReleased: {
                    if (root.document.currentPage > 0 &&
                        currPageDelegate.x > width/6) {
                        switchAnimation.running = true;
                    } else if (root.document.currentPage < document.pageCount-1 &&
                        currPageDelegate.x < -width/6) {
                        switchAnimation.running = true;
                    } else {
                        resetAnim.running = true;
                    }
                    preventStealing = false;
                }
                onCanceled: {
                    resetAnim.running = true;
                    preventStealing = false;
                }
                onDoubleClicked: {
                    flick.contentWidth = flick.width
                    flick.contentHeight = flick.width / mouseArea.currPageDelegate.pageRatio
                }
                onClicked: {
                    if (Math.abs(currPageDelegate.x) < 20) {
                        root.clicked();
                    }
                }

                PageView {
                    id: page1
                    document: root.document
                    z: 2
                }
                PageView {
                    id: page2
                    document: root.document
                    z: 1
                }
                PageView {
                    id: page3
                    document: root.document
                    z: 0
                }

                    
                Binding {
                    target: mouseArea.currPageDelegate
                    property: "pageNumber"
                    value: root.document.currentPage
                }
                Binding {
                    target: mouseArea.currPageDelegate
                    property: "visible"
                    value: true
                }

                Binding {
                    target: mouseArea.prevPageDelegate
                    property: "pageNumber"
                    value: root.document.currentPage - 1
                }
                Binding {
                    target: mouseArea.prevPageDelegate
                    property: "visible"
                    value: !mouseArea.incrementing && root.document.currentPage > 0
                }

                Binding {
                    target: mouseArea.nextPageDelegate
                    property: "pageNumber"
                    value: root.document.currentPage + 1
                }
                Binding {
                    target: mouseArea.nextPageDelegate
                    property: "visible"
                    value: mouseArea.incrementing && root.document.currentPage < document.pageCount-1
                }
                
                SequentialAnimation {
                    id: switchAnimation
                    NumberAnimation {
                        target: mouseArea.currPageDelegate
                        properties: "x"
                        to: mouseArea.incrementing ? -mouseArea.currPageDelegate.width : mouseArea.currPageDelegate.width
                        easing.type: Easing.InQuad
                        //hardcoded number, we would need units from plasma
                        //which cannot depend from here
                        duration: 250
                    }
                    ScriptAction {
                        script: {
                            mouseArea.currPageDelegate.z = 0;
                            mouseArea.prevPageDelegate.z = 1;
                            mouseArea.nextPageDelegate.z = 2;
                        }
                    }
                    ScriptAction {
                        script: {
                            mouseArea.currPageDelegate.x = 0
                            var oldCur = mouseArea.currPageDelegate;
                            var oldPrev = mouseArea.prevPageDelegate;
                            var oldNext = mouseArea.nextPageDelegate;

                            if (mouseArea.incrementing) {
                                root.document.currentPage++;
                                mouseArea.currPageDelegate = oldNext;
                                mouseArea.prevPageDelegate = oldCur;
                                mouseArea. nextPageDelegate = oldPrev;
                            } else {
                                root.document.currentPage--;
                                mouseArea.currPageDelegate = oldPrev;
                                mouseArea.prevPageDelegate = oldCur;
                                mouseArea. nextPageDelegate = oldNext;
                            }
                            mouseArea.currPageDelegate.z = 2;
                            mouseArea.prevPageDelegate.z = 1;
                            mouseArea.nextPageDelegate.z = 0;
                        }
                    }
                }
                NumberAnimation {
                    id: resetAnim
                    target: mouseArea.currPageDelegate
                    properties: "x"
                    to: 0
                    easing.type: Easing.InQuad
                    duration: 250
                }
            }
        }
    }
}