testroot : Elem
{
    Modules : AImports
    {
        $ + /SysComps;
        $ + /IncapsComps;
        $ + /DesComps/Des;
        $ + /DesComps/StcOutExt;
        $ + /DesComps/StcInpExt;
        $ + /GVisComps/GWindow;
        $ + /GVisComps/VisEnv;
        $ + /GVisComps/Scene;
        $ + /GVisComps/SceRect;
    }
    Comps : Elem
    {
        $ # " Vis representation CP ";
              VisrBindCp : ConnPointMc
        {
            $ = "{Provided:'MVisRepr' Required:'MMdlVis'}";
        }
        $ # " Model representation CP ";
        MdlBindCp : ConnPointMc
        {
            $ = "{Provided:'MMdlVis' Required:'MVisRepr'}";
        }
        $ # " Vis representation CP extender ";
        VisrBindExt : AExtd
        {
            Int : ./../MdlBindCp;
        }
    }
    Test : /testroot/Modules/DesComps/Des
    {
        Env : /testroot/Modules/GVisComps/VisEnv
        {
            VisrBind : /testroot/Comps/VisrBindExt;
            VisEnvAgt < Init = Yes;
            Window : /testroot/Modules/GVisComps/GWindow
            {
                AWnd < Init = Yes;
                VisrBind : /testroot/Comps/VisrBindExt;
                Scene : /testroot/Modules/GVisComps/Scene
                {
                    VisrBinder : AVisRepr;
                    Solid : /testroot/Modules/GVisComps/SceRect
                    {
                        WidthCp : /testroot/Modules/DesComps/StcInpExt;
                        HeightCp : /testroot/Modules/DesComps/StcInpExt;
                        Width : AStatec;
                        ./Width < {
                            Debug.Update = y;
                            Value = "SI 20";
                        }
                        Height : AStatec;
                        ./Height < {
                            Debug.Update = y;
                            Value = "SI 60";
                        }
                        ./Width/Inp ~ ./WidthCp/Int;
                        ./Height/Inp ~ ./HeightCp/Int;
                    }
                    VisrBinding : /testroot/Comps/VisrBindCp;
                }
                ./Scene/VisrBinding ~ ./VisrBind/Int;
            }
            Width : AStatec;
            Height : AStatec;
            Title : AStatec;
            Width ~ ./Window/Inp_W;
            Height ~ ./Window/Inp_H;
            Title ~ ./Window/Inp_Title;
            ./Window/VisrBind ~ ./VisrBind/Int;
        }
        ./Env/Width < Value = "SI 600";
        ./Env/Height < Value = "SI 600";
        ./Env/Title < Value = "SS Title";
        $ # "Model being visualized";
        Model : /testroot/Modules/DesComps/Des
        {
            MdlBinder : AMdlVis;
            MdlBinding : /testroot/Comps/MdlBindCp;
            Solid : /testroot/Modules/DesComps/Des
            {
                WidthCp : /testroot/Modules/DesComps/StcOutExt;
                HeightCp : /testroot/Modules/DesComps/StcOutExt;
                Width : AStatec;
                Width < {
                    Debug.Update = y;
                    Value = "SI 40";
                }
                Height : AStatec;
                Height < {
                    Debug.Update = y;
                    Value = "SI 60";
                }
                IncrW : ATrcAddVar;
                IncrH : ATrcAddVar;
                IncrData : AStatec;
                IncrData < Value = "SI 1";
                IncrData ~ ./IncrW/Inp;
                ./IncrW/Out ~ ./Width/Inp;
                Width ~ ./IncrW/Inp;
                IncrData ~ ./IncrH/Inp;
                ./IncrH/Out ~ ./Height/Inp;
                Height ~ ./IncrH/Inp;
                Width ~ ./WidthCp/Int;
                Height ~ ./HeightCp/Int;
            }
        }
        ./Env/VisrBind ~ ./Model/MdlBinding;
    }
}
