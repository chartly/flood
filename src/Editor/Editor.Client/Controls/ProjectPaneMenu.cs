﻿using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Flood.Editor;
using Flood.Editor.Controls;
using EngineManaged.GUI.Control;

namespace Editor.Client.GUI
{
    /// <summary>
    /// 
    /// </summary>
    [Export]
    public class ProjectPaneMenu : EditorTool, DockableTool
    {
        [Import] 
        private ProjectPane projectPane;

        private EngineManaged.GUI.Control.Menu menu; 

        public void InitControl(EngineManaged.GUI.Control.Base parent)
        {
            menu = new Menu(parent);
        }


        public void Show(Project project)
        {
            
        }
    }
}