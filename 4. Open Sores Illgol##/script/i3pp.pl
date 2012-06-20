#!/usr/local/bin/perl -w

# i3pp.pl
# Illberon pre-processor.
# (c)2001 Cat's Eye Technologies.
# All rights reserved.  All liability disclaimed.
# See the file COPING for license information.

# use FileHandle;
$serialNumber = 0;

sub new_FileHandle
{
  return sprintf('FILE%04d', $serialNumber++);
}

%bt = ();
%mr = ();

%comment = ();
%filter  = ('dust', 'pollen');
$pet     = 'cat';

sub ok
{
  my $fname = shift;
  return 0 if exists $bt{$fname};
  $bt{$fname} = 1;
  return 1;
}

sub pp
{
  my $fname = shift;
  my $fh = new_FileHandle();
  if (ok($fname))
  {
    if (not open $fh, "<$fname")
    {
      warn "Could not open $fname"; exit 1;
    }
    while(defined($line = <$fh>))
    {
      my $restart = 1;
      while ($restart)
      {
        $restart = 0;
        my $q;
        foreach $q (sort keys %mr)
        {
          my $qm = quotemeta($q);
          my $m; my $y; my @a; my @b; my $i;
          if ($ma{$q} ne '')
          {
            # print "\*$q, $m, $ma{$q}\*"; <STDIN>;
            while ($line =~ /$qm\s*\((.*?)\)/)
            {
              $y = $1;
              @a = split(/,/, $y);
              @b = split(/,/, $ma{$q});
              $m = $mr{$q};
              for ($i = 0; $i <= $#a; $i++)
              {
                if ($b[$i] =~ /^CDR\s*=\s*(\S*)$/)
                {
                  $b[$i] = $1;
                  $a[$i] =~ s/.$//;
                }
                if ($a[$i] ne '')
                {
                  $m =~ s/$b[$i]/$a[$i]/;
                } else
                {
                  $m = '';
                }
              }
              $line =~ s/$qm\s*\((.*?)\)/$m/;
              # print ">$line\n"; <STDIN>;
              $restart = 1;
            }
          } else
          {
            $restart = ($line =~ s/$qm/$mr{$q}/eg);
            # print ">$line\n";
          }
        }
      }

      my $cd; my $qmcd;
      foreach $cd (keys %comment)
      {
        $qmcd = quotemeta($cd);
        $line =~ s/$cd.*?$//;
      }

      $qmcd = quotemeta($pet);
      $line =~ s/${pet}_hair//;

      foreach $cd (keys %filter)
      {
        $qmcd = quotemeta($cd);
        $line =~ s/$cd//;
      }

      if($line =~ /^\s*INCLUDE\s*=\s*(\S+)/o)
      {
        pp($1);
      }
      elsif($line =~ /^\s*ADD\s*COMMENT\s*=\s*(\S+)/o)
      {
        $comment{$1} = 1;
      }
      elsif($line =~ /^\s*REMOVE\s*COMMENT\s*=\s*(\S+)/o)
      {
        delete $comment{$1};
      }
      elsif($line =~ /^\s*ADD\s*FILTER\s*=\s*(\S+)/o)
      {
        $filter{$1} = 1;
      }
      elsif($line =~ /^\s*DISABLE\s*FILTER\s*(\S+)/o)
      {
        delete $filter{$1};
      }
      elsif($line =~ /^\s*NEW\s*PET\s*=\s*(\S+)/o)
      {
        $pet = $1;
      }
      elsif($line =~ /^\s*DEFINE\s*(\S+?)\s*\((.*?)\)\s*=\s*(.+)$/o)
      {
        $ma{$1} = $2;
        $mr{$1} = $3;
      }
      elsif($line =~ /^\s*DEFINE\s*(\S+)\s*=\s*(.+)$/o)
      {
        $ma{$1} = '';
        $mr{$1} = $2;
      }
      elsif($line =~ /^\s*BLOAD\s*\=\s*(\S+)/o)
      {
        my $g = new_FileHandle(); my $n = 0; my $t = 0;
        if (open $g, "<$1")
        {
          binmode $g;
          print "INLINE {\n";
          while(read($g, $t, 1))
          {
            print ' ' . ord($t);
            $n++;  print "\n  " if $n % 8 == 0;
          }
          close $g;
          print "\n}\n";
        } else
        {
          warn "Could not open $1"; exit 1;
        }
      }
      elsif($line =~ /^\s*bload\s*(\w+)\s*\=\s*\"(.*?)\"/o)
      {
        my $rtn = $1;
        my $fn  = uc $2;
        print
"  * objf$rtn = 0;
  * objl$rtn = 0;
  open(objf$rtn) = \"$fn\";
  eof(objf$rtn) = 1;
  objl$rtn = gpos(objf$rtn,0);
  $rtn = * (objl$rtn);
  seek(objf$rtn by 1) = 0;
  in chunk(objf$rtn, objl$rtn) $rtn^;
  close(objf$rtn) = 1;
";
      }
      elsif($line =~ /^\s*\#H\s*$/o)
      {
        print "print \"Hello, world!\", EoL FIN\n";
      }
      elsif($line =~ /^\s*BEGIN\s*FIELDS\s*\=\s*(\S+)/o)
      {
        my $t = $1; my $st = 0; my $ctr = 0; my $fct = 0;
        while ($line !~ /^\s*END\s*FIELDS/o)
        {
          $line = <$fh>;
          if ($line =~ /(\S+)\s*\=\s*(\d+)/o)
          {
            my $field  = $1;
            my $size   = $2; $st += $size;
            my $offset = 0;
            my $x      = 1;
            my $y      = 1;
            my $desc   = $field;
            if ($line =~ /\@(\d+)/o) { $offset = $1; }
            if ($line =~ /\@\(\s*(\d+)\s*\,\s*(\d+)\s*\)/o) { $y = $1; $x = $2; }
            if ($line =~ /\@\"(.*?)\"/o) { $desc = $1; }

            push @out, "CONST $field = $fct;"; $fct++;
            push @out, "  $t\[${t}Size+$ctr\] BYTE = $offset; NB offset";  $ctr++;
            push @out, "  $t\[${t}Size+$ctr\] BYTE = $size;   NB size";  $ctr++;
            push @out, "  $t\[${t}Size+$ctr\] BYTE = $x;      NB x pos";  $ctr++;
            push @out, "  $t\[${t}Size+$ctr\] BYTE = $y;      NB y pos";  $ctr++;
          }
        }
        print "CONST ${t}Size = $st;\n";
        print "* $t * $st = 0;\n";
        while($#out >= 0)
        {
          print shift(@out) . "\n";
        }
      } else
      {
        print $line;
      }
    }
    close $fh;
  } else { warn "Recursive INCLUDE = sequence"; }
}

pp($ARGV[0]);

### END of i3pp.pl ###

