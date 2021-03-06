//
//  SimpleViewController.mm
//  VinezorsiOS
//
//  Created by Calvin Phung on 11/3/13.
//
//

#import "SimpleMenuViewController.h"
#import "AppDelegate.h"

@interface SimpleMenuViewController ()

@property (retain, nonatomic) IBOutlet UITextField *TextField_Input;
@property (retain, nonatomic) IBOutlet UISwitch *Switch_Music;

@end

@implementation SimpleMenuViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)dealloc {
    [_TextField_Input release];
    [super dealloc];
}

- (NSUInteger)supportedInterfaceOrientations {
    NSUInteger mask = 0;
    return mask | UIInterfaceOrientationMaskLandscape;
}

- (BOOL)shouldAutoRotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    return interfaceOrientation == UIInterfaceOrientationLandscapeLeft || interfaceOrientation == UIInterfaceOrientationLandscapeRight;
}

- (BOOL)shouldAutorotate {
    UIInterfaceOrientation orientation = [[UIApplication sharedApplication] statusBarOrientation];
    
    return (orientation == UIInterfaceOrientationLandscapeLeft) || (orientation == UIInterfaceOrientationLandscapeRight);
}

- (IBAction)buttonBegin_OnPress:(UIButton *)sender {
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    
    [appDelegate go:_TextField_Input.text];
}

@end
