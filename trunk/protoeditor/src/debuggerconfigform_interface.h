#ifndef __DEBUGGERCONFIGFORM_INTERFACE_H
#define __DEBUGGERCONFIGFORM_INTERFACE_H

class DebuggerConfigFormInterface
{
public:
    DebuggerConfigFormInterface() {}
    virtual ~DebuggerConfigFormInterface() {}

    virtual void canceled() = 0;
    virtual void confirmed() = 0;

private:
    DebuggerConfigFormInterface( const DebuggerConfigFormInterface& source );
    void operator = ( const DebuggerConfigFormInterface& source );
};


#endif // __DEBUGGERCONFIGFORM_INTERFACE_H
