﻿using System;
using System.ComponentModel.Composition;

namespace Flood.Editor.GUI.Controls
{
    class ToolNewDoc : EditorTool, BarTool
    {
        [Import]
        DocumentManager docManager;

        public void OnSelect()
        {
            Console.WriteLine("Open windows select doc type and path");
            docManager.Open("dummy");
        }

        public string Text
        {
            get { return "New Document"; }
        }

        public string Image
        {
            get { return "new.png"; }
        }
    }
}