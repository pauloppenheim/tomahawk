#import <Foundation/Foundation.h>
#import <ScriptingBridge/ScriptingBridge.h>
#import "adium.h"

void script( const char* status )
{
  NSString *stat = [NSString stringWithUTF8String:status];
  NSAppleScript *appleScript = [[NSAppleScript alloc] initWithSource:stat];
  NSDictionary *errorDictionary;
  NSAppleEventDescriptor *eventDescriptor = [appleScript executeAndReturnError:&errorDictionary];
}
