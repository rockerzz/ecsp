#!/usr/bin/perl -w
#

use strict;
use warnings;


#if ($string =~ s/\/\/(\s*)Автор:(\s*)(.*\n)/\/\/ Author: rockerzz/) { print "found: $string\n"; }


getDir('./testsuite');  

sub getDir
{  
    my($dir,@content,$subdir,@all_files, $change);  
    $dir = shift;  
    if(! -d $dir)
    {
        return;
    }
    opendir(DIR,$dir) or die "Can't open folder $dir";  
    @content = grep { !/^\.{1,2}$/} readdir(DIR);  
    closedir(DIR);  
    foreach $subdir (@content)
    {  
        if($subdir =~ /^\w+$/)
        {  
            @all_files = $dir;  
            getDir("$dir/$subdir");  
        }
        else
        {  
            print "$dir/$subdir\n";  

            if(-f "$dir/$subdir") 
            {

            open(FILE, "+< $dir/$subdir") or die $!;
            my $out = "";
            while(<FILE>)
            {
                no warnings 'uninitialized';

                #s/using namespace ecsp::core\;//eg;
                #s/get_instance/instance/eg;
                s/DbField/DBField/g;
                #s/ecsp::core:://eg;
                #s/core:://eg;

                #if(/ECSP_NAMESPACE_BEGIN\(core\)/i)
                #{
                #    $change = 1;
                #}

                #if($change == 1)
                #{
                #    s/ECSP_NAMESPACE_BEGIN\(core\)/namespace ecsp \{/g;
                #   s/ECSP_NAMESPACE_END/\} \/\/ namespace ecsp/g;
                #}

                #if(/namespace ecsp \{/i)
                #{
                #    $change = 1;
                #}

                #if($change == 1)
                #{
                #    s/ECSP_NAMESPACE_BEGIN\(core\)/namespace ecsp \{/g;
                #    s/ECSP_NAMESPACE_END/\} \/\/ namespace ecsp/g;
                #}


                $out .= $_;
            }

            $change = 0;
            seek(FILE, 0, 0);
            print FILE $out;
            truncate(FILE, tell(FILE));
            close(FILE);
            }
        }  
    }  
}
