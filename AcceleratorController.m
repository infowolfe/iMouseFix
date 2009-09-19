#import "AcceleratorController.h"
#import "mousefix.h"

static BOOL isQuitting = NO;

@implementation AcceleratorController
- (id) init {
	//NSLog(@"%s", __PRETTY_FUNCTION__);
	if (self = [super init]) {
		NSUserDefaults *ud = [NSUserDefaults standardUserDefaults];
		NSDictionary *defaults = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSNumber numberWithDouble:3.0], @"MouseAcceleration",
			[NSNumber numberWithBool:YES], @"EnableMouseAcceleration", nil];
		[ud registerDefaults:defaults];
		[ud addObserver:self forKeyPath:@"MouseAcceleration" options:nil context:nil];
		[ud addObserver:self forKeyPath:@"EnableMouseAcceleration" options:nil context:nil];
		[NSApp setDelegate:self];
	}
	return self;
}
- (IBAction)reset:(id)sender {
	NSUserDefaults *ud = [NSUserDefaults standardUserDefaults];
	[ud setValue:[NSNumber numberWithDouble:3.0] forKey:@"MouseAcceleration"];
	[ud setValue:[NSNumber numberWithBool:YES] forKey:@"EnableMouseAcceleration"];
	[self runMousefix];
}
- (void) runMousefix {
	NSUserDefaults *ud = [NSUserDefaults standardUserDefaults];
	BOOL enabled = [[ud objectForKey:@"EnableMouseAcceleration"] boolValue];
	if (enabled==YES) {
		double acceleration = [[ud objectForKey:@"MouseAcceleration"] doubleValue];
		NSString *exestr = [NSString stringWithFormat:@"%f", acceleration];
		const char *argv[2];
		argv[0] = "imousefix";
		argv[1] = [exestr UTF8String];
		imousefix(2, argv);
	} else {
		const char *argv[1];
		argv[0] = "imousefix";
		imousefix(1, argv);
	}		 
}
- (void) observeValueForKeyPath:(NSString *)keyPath
		ofObject:(id)object 
		change:(NSDictionary *)change
		context:(void *)context {
	[self runMousefix];
}
- (void)windowWillClose:(NSNotification *)aNotification {
	if (isQuitting==NO) {
		isQuitting = YES;
		[NSApp terminate:self];
	}
}
- (void)applicationWillTerminate:(NSNotification *)aNotification {
	NSLog(@"%s", __PRETTY_FUNCTION__);
	NSUserDefaults *ud = [NSUserDefaults standardUserDefaults];
	[ud synchronize];
}
@end
