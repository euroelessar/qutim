function Component()
{
}

Component.prototype.isDefault = function()
{
    // select the component by default
    return true;
}

Component.prototype.createOperations = function()
{
    try {
        // call the base create operations function
        component.createOperations();
        if(installer.value("os") == "win")
            component.addOperation("CreateShortcut", "@TargetDir@/bin/qutIM", "@StartMenuDir@/qutIM.lnk");
    } catch (e) {
        print(e);
    }
}
